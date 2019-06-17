/*
 * t_sigsuspend.c
 * 
 * provided by tlpi
 */

#define _GNU_SOURCE
#include <string.h>
#include <signal.h>
#include <time.h>

#include <signals/signal_functions.h>
#include <lib/tlpi_hdr.h>

static volatile sig_atomic_t got_sig_quit = 0;

static void
handler(int sig)
{
    printf("caught signal %d (%s)\n", sig, strsignal(sig));
    if (sig == SIGQUIT)
        got_sig_quit = 1;
}

int
main(int argc, char *argv[])
{
    int loop_num;
    time_t start_time;
    sigset_t orig_mask;
    sigset_t block_mask;
    struct sigaction sa;

    printSigMask(stdout, "Initial signal mask is:\n");

    sigemptyset(&block_mask);
    sigaddset(&block_mask, SIGINT);
    sigaddset(&block_mask, SIGQUIT);
    if(sigprocmask(SIG_BLOCK, &block_mask, &orig_mask) == -1) {
        errExit("sigprocmask - SIG_BLOCK");
    }

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = handler;
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        errExit("sigaction");
    }
    if (sigaction(SIGQUIT, &sa, NULL) == -1) {
        errExit("sigaction");
    }

    for (loop_num = 1; !got_sig_quit; loop_num++) {
        printf("=== LOOP %d\n", loop_num);

        /* simulate a critical section by delaying a few seconds */
        printSigMask(stdout, "Starting critical section, signal mask is:\n");
        for (start_time = time(NULL); time(NULL) < start_time + 4; )
            continue;

        printPendingSigs(stdout, "Before sigsuspend() - pending signals:\n");
        
        if (sigsuspend(&orig_mask) == -1 && errno != EINTR)
            errExit("sigsuspend");
    }

    if (sigprocmask(SIG_SETMASK, &orig_mask, NULL) == -1) 
        errExit("sigprocmask - SIG_SETMASK");
    
    printSigMask(stdout, "=== Exited loop\nResotred signal mask to:\n");

    /* do something... */
    exit(EXIT_SUCCESS);
}
