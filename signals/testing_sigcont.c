/*
 * testing_sigcont.c
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>

void sigcont_handler(int sig)
{
    printf("handling sig %s\n", strsignal(sig));
    return;
}

int
main()
{
    struct sigaction sa;
    sa.sa_handler = sigcont_handler;
    sa.sa_flags = 0; 
    
    sigset_t new_mask;
    sigset_t prev_mask;
    sigaddset(&new_mask, SIGCONT);

    if (sigfillset(&sa.sa_mask) == -1) {
        fprintf(stderr, "sigfillset\n");
        exit(EXIT_FAILURE);
    }

    if (sigaction(SIGCONT, &sa, NULL) == -1) {
        fprintf(stderr, "sigaction\n");
        exit(EXIT_FAILURE);
    }
    
    printf("Blocking SIGCONT\n");
    if (sigprocmask(SIG_BLOCK, &new_mask, &prev_mask) == -1) {
        fprintf(stderr, "sigprocmask\n");
        exit(EXIT_FAILURE);
    }
    
    printf("sleeping for 30s\n");
    sleep(30);

    printf("Unblocking SIGCONT\n");
    if(sigprocmask(SIG_SETMASK, &prev_mask, NULL) == -1) {
        fprintf(stderr, "sigprocmask\n");
        exit(EXIT_FAILURE);
    }
    
    exit(EXIT_SUCCESS);
}
