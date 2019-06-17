/*
 * sig_nodefer.c
 *
 * Examining SA_NODEFER.
 *
 * 31 January 2019
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <errno.h>

void 
sig_handler(int sig)
{
    sigset_t curr_set;
    sigprocmask(SIG_BLOCK, NULL, &curr_set);

    if (sigismember(&curr_set, sig)) {
        printf("%s is in current sigmask\n", strsignal(sig));
    } else {
        printf("%s is not in current sigmask\n", strsignal(sig));
    }
}

int
main()
{
    int rc;
    struct sigaction sigint_act;
    struct sigaction sigterm_act;

    memset(&sigint_act, 0, sizeof(struct sigaction));
    memset(&sigterm_act, 0, sizeof(struct sigaction));

    sigemptyset(&sigint_act.sa_mask);
    sigemptyset(&sigterm_act.sa_mask);

    sigint_act.sa_flags = SA_NODEFER;
   
    sigint_act.sa_handler = sig_handler;
    sigterm_act.sa_handler = sig_handler;

    rc = sigaction(SIGINT, &sigint_act, NULL);
    if (rc == -1) {
        fprintf(stderr, "ERROR: sigaction SIGINT -- %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    rc = sigaction(SIGTERM, &sigterm_act, NULL);
    if (rc == -1) {
        fprintf(stderr, "ERROR: sigaction SIGTERM -- %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    
    for (;;) {
        printf("Sleeping\n");
        sleep(5);
    }

    exit(EXIT_SUCCESS);
}
