/*
 * signal_functions.c
 */

#define _GNU_SOURCE
#include <string.h>
#include <signal.h>

#include <signals/signal_functions.h>
#include <lib/tlpi_hdr.h>

/* note: all the following use fprintf(), which is 
 *       not async-signal-safe (see Section 21.1.2).
 *       As such, these functions are not 
 *       async-signal-safe (i.e., beware of indiscriminately
 *       calling them from signal handlers.
 */

/* print list of signals within a signal set */
void 
printSigset(FILE *of, const char *prefix, const sigset_t *sigset)
{
    int sig;
    int cnt;

    cnt = 0;
    for (sig = 1; sig < NSIG; sig++) {
        if (sigismember(sigset, sig)) {
            cnt++;
            fprintf(of, "%s%d (%s)\n", prefix, sig, strsignal(sig));
        }
    }

    if (cnt == 0) {
        fprintf(of, "%s<empty signal set>\n", prefix);
    }
}

/* print mask of blocked signals for this process */
int
printSigMask(FILE *of, const char *msg)
{
    sigset_t currMask;
    
    if (msg != NULL) {
        fprintf(of, "%s", msg);
    }
    
    if (sigprocmask(SIG_BLOCK, NULL, &currMask) == -1) {
        return -1;
    }

    printSigset(of, "\t\t", &currMask);

    return 0;
}

/* print signals currently pending for this process */
int
printPendingSigs(FILE *of, const char *msg)
{
    sigset_t pendingSigs;

    if (msg != NULL) {
        fprintf(of, "%s", msg);
    }

    if (sigpending(&pendingSigs) == -1) { 
        return -1;
    }

    printSigset(of, "\t\t", &pendingSigs);

    return 0;
}
