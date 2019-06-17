/* 
 * sig_receiver.c
 *
 * Provided by TLPI
 */

#define _GNU_SOURCE
#include <signal.h>

#include <signals/signal_functions.h>
#include <lib/tlpi_hdr.h>

static int sig_cnt[NSIG];
static volatile sig_atomic_t got_sigint = 0;

static void
handler(int sig)
{
    if (sig == SIGINT)
        got_sigint = 1;
    else 
        sig_cnt[sig]++;
}

int 
main(int argc, char *argv[])
{
    int n;
    int num_secs;

    sigset_t pending_mask;
    sigset_t blocking_mask;
    sigset_t empty_mask;
    
    struct sigaction act;


    printf("%s: PID is %ld\n", argv[0], (long)getpid());

    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    act.sa_handler = handler;
    for (n = 1; n < NSIG; n++) {
        if (sigaction(n, &act, NULL) == -1) {
            errExit("sigaction");
        }
    }

    /*
     * if a sleep time was specified, temporarily block all signals,
     * sleep (while another process sends us signals), and then
     * display the mask of pending signals and unblock all signals.
     */

    if (argc > 1) {
        num_secs = getInt(argv[1], GN_GT_0, NULL);

        sigfillset(&blocking_mask);
        if(sigprocmask(SIG_SETMASK, &blocking_mask, NULL) == -1) {
            errExit("sigprocmask");
        }

        printf("%s: sleepding for %d secodns\n", argv[0], num_secs);
        sleep(num_secs);

        if (sigpending(&pending_mask) == -1) {
            errExit("sigpending");
        }

        printf("%s: pending signals are \n", argv[0]);
        printSigset(stdout, "\t\t", &pending_mask);

        sigemptyset(&empty_mask);
        if (sigprocmask(SIG_SETMASK, &empty_mask, NULL) == -1) {
            errExit("sigprocmask");
        }
    }

    while (!got_sigint)
        continue;

    for (n = 1; n < NSIG; n++) {
        if (sig_cnt[n] != 0) {
            printf("%s: signal %d caught %d time%s\n", argv[0], n,
                    sig_cnt[n], (sig_cnt[n] == 1) ? "" : "s");
        }
    }

    exit(EXIT_SUCCESS);
}
