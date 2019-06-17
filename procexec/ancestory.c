/*
 * ancestory.c
 *
 * determining when a process is inheirtied by 
 * init.
 *
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <errno.h>

#include <sys/wait.h>

void sigterm_handler(int sig)
{
    printf("sigterm received\n");
    _exit(EXIT_SUCCESS);
}

int main()
{
    int rc;
    int status;
    int counter = 0;

    setbuf(stdout, NULL);

    switch (fork()) {
        case -1:
            exit(EXIT_FAILURE);
        case 0:
            /* parent */
            switch (fork()) {
                case -1:
                    _exit(EXIT_FAILURE);
                case 0:
                    /* grand child */
                    for (;;) {
                        printf("parent: %ld\n", (long)getppid());
                        if (getppid() == 1) {
                            printf("inheirited by init\n");
                            _exit(EXIT_FAILURE);
                        }
                        counter++;
                        if (counter == 5) {
                            printf("Child killing parent!\n");
                            kill(getppid(), SIGTERM);
                        }
                        sleep(2);
                    }
                default:
                    printf("parent waiting for death...\n");
                    struct sigaction sa;
                    sa.sa_handler = sigterm_handler;
                    sa.sa_flags = 0;
                    sigemptyset(&sa.sa_mask);
                    pause();
            }
        default:
            /* grand parent */
            printf("Grand parent -- waiting for parent\n");
            sleep(30);
            rc = wait(&status);
            if (rc == -1) {
                fprintf(stderr, "wait -- %s\n", strerror(errno));
                exit(EXIT_FAILURE);
            }
    }
}
