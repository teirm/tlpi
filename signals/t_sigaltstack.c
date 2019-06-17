/*
 * t_sigaltstack.c
 *
 * Running a signal handler on an alternate stack.
 */

#define _GNU_SOURCE
#include <string.h>
#include <signal.h>
#include <lib/tlpi_hdr.h>

static void
sigsegv_handler(int sig)
{
    int x;

    printf("caught signal %d (%s)\n", sig, strsignal(sig));
    printf("top of handler near     %10p\n", (void *) &x);
    fflush(NULL);

    _exit(EXIT_FAILURE);    /* not able to return after SIGSEGV */
}

static void 
overflow_stack(int call_num)
{
    char a[100000];

    printf("call %4d - top of stack near %10p\n", call_num, &a[0]);
    overflow_stack(call_num + 1);
}

int
main(int argc, char *argv[])
{
    stack_t sigstack;
    struct sigaction sa;
    int j;

    printf("top of standard stack is near %10p\n", (void *)&j);

    /* 
     * allocate alternate stacka nd inform kernel of its
     * existence.
     */

    sigstack.ss_sp = malloc(SIGSTKSZ);
    if (sigstack.ss_sp == NULL)
        errExit("malloc");

    sigstack.ss_size = SIGSTKSZ;
    sigstack.ss_flags = 0;
    if(sigaltstack(&sigstack, NULL) == -1) {
        errExit("sigaltstack");
    }
    printf("alternate stack is at       %10p-%p\n",
            sigstack.ss_sp, (char *)sbrk(0) - 1);

    sa.sa_handler = sigsegv_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_ONSTACK;
    if (sigaction(SIGSEGV, &sa, NULL) == -1)
        errExit("sigaction");

    overflow_stack(1);
}
