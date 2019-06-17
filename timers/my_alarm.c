/*
 *
 * my_alarm.c 
 *
 * implementation of alarm system call
 * using setitimer.
 *
 * 09 February 2019
 */

#include <unistd.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>

#include <sys/time.h>

static void
sigalrm_handler(int sig)
{
    printf("caught sigalrm\n");
}

unsigned my_alarm(unsigned seconds)
{
    int rc;
    struct itimerval timer;

    memset(&timer, 0, sizeof(struct itimerval));   

    rc = getitimer(ITIMER_REAL, &timer);
    if (timer.it_value.tv_sec != 0) { } 

    timer.it_value.tv_sec = seconds;
    timer.it_value.tv_usec = 0;
    
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 0;

    setitimer(ITIMER_REAL, &timer, NULL);
    
    return 0;
}


int
main()
{
    
    struct sigaction sa;

    sa.sa_handler = sigalrm_handler;
    sa.sa_flags = 0;
    sigfillset(&sa.sa_mask);

    if (sigaction(SIGALRM, &sa, NULL) == -1) {
        fprintf(stderr, "ERROR: sigaction\n");
        exit(EXIT_FAILURE);
    }

    printf("Setting alarm\n");
    my_alarm(5);
    
    pause();
}
