/*
 * sig_resethand.c 
 *
 * Understand the behavior of 
 * SIG_RESETHAND flag for sigaction
 *
 * 31 January 2018
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

volatile sig_atomic_t caught_sig;

void sig_handler(int sig)
{
    printf("Signal Caught: %s\n", strsignal(sig));
    caught_sig = 1;
}

int
main()
{
    int rc; 
    struct sigaction sa;
     
    memset(&sa, 0, sizeof(sigaction));
    
    sigfillset(&sa.sa_mask);
    sa.sa_handler = sig_handler;
    sa.sa_flags = SA_RESETHAND;
    
    rc = sigaction(SIGINT, &sa, NULL);
    if (rc == -1) {
        fprintf(stderr, "ERROR: sigaction -- %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    for (;;) {
        sleep(3);
        printf("Sleeping\n");
    }

    exit(EXIT_SUCCESS);
}
