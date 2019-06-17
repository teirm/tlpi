/*
 * dead_parent.c
 *
 * Verify that the death of a parent makes init
 * the parent (pid 1).
 *
 * 15 February 2019
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <errno.h>

void sigterm_handler(int sig)
{
    printf("received sig: %d\n", sig);
    exit(EXIT_SUCCESS);
}

int main()
{
    int counter = 0;
    int rc;
    
    struct sigaction sa;

    setbuf(stdout, NULL);

    switch (fork()) {
        case -1:
            exit(EXIT_FAILURE);
        case 0:
            /* child */
            for  (;;) {
                printf("Parent pid: %ld\n", (long)getppid());
                if (getppid() == 1) {
                    printf("Inherited by init\n"); 
                    _exit(EXIT_SUCCESS);
                }
                counter++;
                if (counter == 10) {
                    rc = kill(getppid(), SIGTERM);
                    if (rc != 0) {
                        fprintf(stderr, "killing failed with %s\n", strerror(errno));
                    }
                }
                sleep(3);
            }
            break;
        default:
            /* parent */
            printf("parent process -- doing nothing\n");
            sa.sa_flags = 0;
            sa.sa_handler = sigterm_handler;
            sigemptyset(&sa.sa_mask);
            pause();
    }
}
