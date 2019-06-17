/* 
 * blocked_sigchld.c
 *
 * Seeing the behavior of a blocked sigchld
 * and wait. When a signal handler is installed
 * in the parent.
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <errno.h>

#include <sys/wait.h>


void sigchld_handler(int sig)
{
    printf("received %s\n", strsignal(sig));
}

int main()
{
    int status;
    pid_t child_pid; 
    sigset_t new_set; 
    sigset_t old_set;

    struct sigaction sa;
    sa.sa_flags = 0;
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        fprintf(stderr, "sigaction\n");
        exit(EXIT_FAILURE); 
    }

    sigaddset(&new_set, SIGCHLD);
    sigprocmask(SIG_BLOCK, &new_set, &old_set);
   
    setbuf(stdout, NULL);

    switch (child_pid = fork()) {
    case -1:
        fprintf(stderr, "failed to fork\n");
        exit(EXIT_FAILURE);
    case 0:
        /* child */
        sigprocmask(SIG_SETMASK, &old_set, NULL);
        printf("In child\n");
        _exit(EXIT_SUCCESS);
    default:
        printf("in parent\n");
        if (wait(&status) == -1) {
            fprintf(stderr, "wait failed\n");
            exit(EXIT_FAILURE);
        }
        printf("Received wait status: %d\n", status);
        printf("unblocking SIGCHLD\n");
        sigprocmask(SIG_SETMASK, &old_set, NULL);
        exit(EXIT_SUCCESS);
    }
}
