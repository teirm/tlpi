/*
 * timer_create_defaults.c
 *
 * A program to verify the defaults
 * of the timer_create call if the evp 
 * field is NULL.
 *
 * 10 February 2019
 */

#include <signal.h>
#include <time.h>
#include <string.h>

#include <lib/tlpi_hdr.h>

static void handler(int sig, siginfo_t *si, void *uc)
{
    printf("received %s\n", strsignal(sig));
    printf("timer_id: %d\n", si->si_value.sival_int);
    return;
}

int main(int argc, char *argv[])
{
    int j;
    struct sigaction sa;
    timer_t timer;
    struct itimerspec ts;

    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = handler;
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGALRM, &sa, NULL) == -1) {
        errExit("sigaction");
    }

    if (timer_create(CLOCK_REALTIME, NULL, &timer) == -1) {
        errExit("timer_create");
    }
    
    ts.it_interval.tv_sec = 0;
    ts.it_interval.tv_nsec = 0;

    ts.it_value.tv_sec = 5;
    ts.it_value.tv_nsec = 0;

    if (timer_settime(timer, 0, &ts, NULL) == -1) {
        errExit("timer_settime");
    }

    pause();

    exit(EXIT_SUCCESS);
}
