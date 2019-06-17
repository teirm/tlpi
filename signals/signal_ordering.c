/*
 * signal_ordering.c
 *
 * ordering of realtime and regular
 * signals when delivered to a process 
 * blocking both.
 *
 * 07 February 2019
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

static void 
sig_handler(int sig)
{
    printf("caught signal %d\n", sig);
}


int
main()
{
    int sig;
    struct sigaction sa;
    sigset_t new_mask;
    sigset_t prev_mask;

    sigfillset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = sig_handler;

    for (sig = 1; sig < NSIG; sig++) {
        if (sig != SIGTSTP && sig != SIGQUIT) {
            sigaction(sig, &sa, NULL);
        }
    }

    sigfillset(&new_mask);
    sigprocmask(SIG_BLOCK, &new_mask, &prev_mask);

    printf("sleeping for 30\n");
    sleep(60);

    printf("clearing signal mask\n");
    sigprocmask(SIG_SETMASK, &prev_mask, NULL);

    exit(EXIT_SUCCESS);
}
