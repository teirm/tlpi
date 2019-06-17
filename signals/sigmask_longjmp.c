/*
 * sigmask_longjmp.c
 *
 * Non-local goto from a signal handler.
 */

#define _GNU_SOURCE
#include <string.h>
#include <setjmp.h>
#include <signal.h>

#include <signals/signal_functions.h>
#include <lib/tlpi_hdr.h>

/* set to 1 once env buffer has been initialized by [sig]setjmp() */
static volatile sig_atomic_t can_jump = 0;

#ifdef USE_SIGSETJMP
static sigjmp_buf s_env;
#else
static jmp_buf env;
#endif

static void
handler(int sig)
{
    /* uses unsafe non-async-signal-safe functions */
    printf("received signal %d (%s), signal mask is:\n", sig, strsignal(sig));
    printSigMask(stdout, NULL);

    if (!can_jump) {
        printf("'env' buffer not yet set, doing a simple return\n");
        return;
    }

#ifdef USE_SIGSETJMP
    siglongjmp(s_env, 1);
#else
    longjmp(env, 1);
#endif
}

int
main(int argc, char *argv[])
{
    struct sigaction sa;

    printSigMask(stdout, "signal mask at startup:\n");

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = handler;
    if(sigaction(SIGINT, &sa, NULL) == -1)
        errExit("sigaction");
    
#ifdef USE_SIGSETJMP
    printf("calling sigsetjmp()\n");
    if (sigsetjmp(s_env, 1) == 0) {
#else
    printf("calling setjmp()\n");
    if (setjmp(env) == 0) {
#endif
        can_jump = 1;
    } else {
        printSigMask(stdout, "After jump from handler, sig mask is:\n");
    }

    for (;;)
        pause();
}
