/* concurrent forking server */

#include <signal.h>
#include <syslog.h>
#include <sys/wait.h>

#include <daemons/become_daemon.h>
#include <sockets/inet_sockets.h>
#include <lib/tlpi_hdr.h>

#define SERVICE         "echo"
#define BUF_SIZE        4096
#define BACKLOG         10
#define MAX_CHILDREN    20 

sig_atomic_t child_count = 0; 

/* reap the kiddies once they die */
static void
grim_reaper(int sig)
{
    int saved_errno = errno;
    
    /* 
     * WNOHANG -- return immediately if no child has 
     * exited.
     */
    while (waitpid(-1, NULL, WNOHANG) > 0) {
        continue;
    }
    errno = saved_errno;
    /* TODO: is this racy? */
    child_count--;
}

/* handle client request: copy socket input back to socket */
static void
handle_request(int client_fd)
{
    char buf[BUF_SIZE];
    ssize_t num_read = 0;

    while ((num_read = read(client_fd, buf, BUF_SIZE)) > 0) {
        if (write(client_fd, buf, num_read) != num_read) {
            syslog(LOG_ERR, "write() failed: %s", strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    if (num_read == -1) {
        syslog(LOG_ERR, "Error from read(): %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
}

int
main(int argc, char *argv[]) 
{
    int listen_fd = 0;
    int client_fd = 0;
    struct sigaction sa;
    sigset_t orig_mask;
    sigset_t block_mask;

    if (becomeDaemon(0) == -1) {
        errExit("becomeDaemon");
    }

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = grim_reaper;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        syslog(LOG_ERR, "Error from sigaction(): %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    listen_fd = inet_listen(SERVICE, BACKLOG, NULL);
    if (listen_fd == -1) {
        syslog(LOG_ERR, "could not create server socket (%s)", strerror(errno));
        exit(EXIT_FAILURE);
    }
    
    for (;;) {
        client_fd = accept(listen_fd, NULL, NULL);
        if (client_fd == -1) {
            syslog(LOG_ERR, "failure in accept(): %s", strerror(errno));
            exit(EXIT_FAILURE);
        }
    
        /* handle each client request in a new child process */
        switch (fork()) {
        case -1:
            /* give up on this client -- might be a temporary failure */
            syslog(LOG_ERR, "can't create child (%s)", strerror(errno));
            close(client_fd);
            break;
        case 0:
            /* child */
            if (child_count < MAX_CHILDREN) {
                close(listen_fd);
                handle_request(client_fd);
            }
            _exit(EXIT_SUCCESS);
        default:
            sigemptyset(&block_mask);
            sigaddset(&block_mask, SIGCHLD);
            if (sigprocmask(SIG_BLOCK, &block_mask, &orig_mask) == -1) {
                errExit("sigprocmask - SIG_BLOCK");
            }
            /* parent */
            if (child_count < MAX_CHILDREN) {
                child_count++;
            }
            if (sigprocmask(SIG_SETMASK, &orig_mask, NULL) == -1) {
                errExit("sigprocmask - SIG_SETMASK");
            }
            close(client_fd);
            break;
        }
    }
}
