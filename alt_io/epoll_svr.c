/**
 * A server written to handle connections
 * using epoll.
 *
 * 27 Feb 2021
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>

#include <sys/epoll.h>
#include <sys/socket.h>

#include <sockets/inet_sockets.h>

#define BACKLOG     10      /* TCP socket backlog */
#define MAX_EVENTS  10      /* Maximum events to be retruned in single epoll_wait() */
#define MAX_BUF     500     /* Maximum bytes fetched in a read */

#define memzero(_p_,_size_) memset((_p_),0,(_size_))

int running;

typedef struct {
    int listen_fd;
    int epoll_fd;
} listener_args_t;

/**
 * @brief thread to listen and accept connections and put 
 *        them on the epoll list
 *
 * @arg[in] args    listener args 
 */
static void *listener(void *arg)
{
    assert(arg != NULL);
   
    listener_args_t *listener_args = (listener_args_t *)arg;  
    int listen_fd = listener_args->listen_fd;
    int epoll_fd  = listener_args->epoll_fd;
    int client_fd = 0; 
    int res       = 0; 

    struct epoll_event ev;

    while (running) {
        memzero(&ev, sizeof(ev));
        client_fd = accept(listen_fd, NULL, NULL); 
        if (client_fd == -1) {
            /* 
             * If a connection fails -- just log a message and 
             * move on. Not a lot else can be done.
             */
            perror("accept");
        }

        ev.events   = EPOLLIN;
        ev.data.fd  = client_fd;
    
        /* 
         * TODO:
         * what happens to all fds added to epoll if the epoll instance closes,
         * do they need to be tracked elsewhere? Probably stay open.
         * and need to be closed separately.
         */
        res = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev);
        if (res == -1) {
            perror("epoll_ctl");
            close(client_fd);
        }
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    int epoll_fd     = 0;
    int ready_count  = 0;
    int listen_fd    = 0;
    int res          = 0;
    
    char   read_buffer[MAX_BUF];
    struct epoll_event evlist[MAX_EVENTS];
    listener_args_t thread_args;
    
    pthread_t        listener_tid; 

    memzero(read_buffer, sizeof(read_buffer));
    memzero(evlist, sizeof(evlist));
    memzero(&thread_args, sizeof(thread_args));
    memzero(&listener_tid, sizeof(listener_tid));

    if (argc != 2) {
        fprintf(stderr, "usage: ./%s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    /* 
     * epoll create ignores the value on linux, but it 
     * should be non zero
     */
    epoll_fd = epoll_create(argc - 1);
    if (epoll_fd == -1) {
        perror("epoll_create");
        exit(EXIT_FAILURE);
    }
    
    listen_fd = inet_listen(argv[1], BACKLOG, NULL);
    if (listen_fd == -1) {
        perror("inet_listen");
        exit(EXIT_FAILURE);
    }

    thread_args.listen_fd = listen_fd;
    thread_args.epoll_fd  = epoll_fd;

    res = pthread_create(&listener_tid, NULL, listener, &thread_args);     
    if (res == -1) {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }
    
    /* the server can now accept connections */
    running = 1;
    while (running) {
        /* epoll_wait uses a timeout of -1 to indicate indefinite block */
        ready_count = epoll_wait(epoll_fd, evlist, MAX_EVENTS, -1); 
        if (ready_count == -1) {
            if (errno == EINTR) {
                continue;
            } else {
                perror("epoll_wait");
                /* 
                 * an error on epoll wait indicates something might be 
                 * really wrong with the server (really?) and we should
                 * just log and shutdown.
                 */
                break;
            }
        }  
        for (int i = 0; i < ready_count; ++i) {
            /* 
             * handle events here -- in reality we don't 
             * want to be processing events here that might block the
             * epoll loop. It would be better to throw them onto a queue
             * for a thread or a thread pool to work on.
             */
            if (evlist[i].events & EPOLLIN) {
                /* 
                 * TODO: do the dumb thing now and read off the input in the
                 * same thread as the epoll. The smarter thing is a thread pool
                 * but I (a) don't have that written in C right now and (b)
                 * don't have adequate hardware to test many threads
                 */
                res = read(evlist[i].data.fd, read_buffer, MAX_BUF-1);
                
                /* This check is needed in the case that the client 
                 * has closed the connection. subsequent reads will return
                 * zero always triggering EPOLLIN. EPOLLHUP is sent
                 * only on shutdown(rdwr)
                 */
                if (res == 0) {
                    /* client closed connection */
                    if (close(evlist[i].data.fd) == -1) {
                        perror("close");
                    }
                }
                if (res == -1) {
                    perror("read");
                }
                printf("%s", read_buffer);
                memzero(read_buffer, sizeof(read_buffer));
            } 
        }
    }

    /* shutdown path */
    running = 0;
    
    /* join on listener prior to closing epoll fd */
    res = pthread_join(listener_tid, NULL);
    if (res == -1) {
        perror("pthread_join");
    }
    
    /* close listener fd */
    if (close(listen_fd) == -1) {
        perror("close");
    }

    if (close(epoll_fd) == -1) {
        perror("close");
    }
    return 0;
}
