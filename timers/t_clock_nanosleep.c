/*
 * t_clock_nanosleep.c
 *
 * Implementation of t_nanosleep.c 
 * provided by TLPI.
 *
 * 09 February 2019
 */

#include <time.h>
#include <signal.h>

#include <sys/time.h>

#include <lib/tlpi_hdr.h>

/* just interrupt nanosleep() */
static void
sigint_handler(int sig)
{
    return;
}

int 
main(int argc, char *argv[])
{   
    struct timeval start;
    struct timeval finish;

    struct timespec request;
    struct sigaction sa; 
    int s;

    if (argc != 3 || strcmp(argv[1], "--help") == 0) {
        usageErr("%s secs nanosecs\n", argv[0]);
    }
    
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = sigint_handler;
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        errExit("sigaction");
    }

    if (clock_gettime(CLOCK_REALTIME, &request) == -1) {
        errExit("clock_gettime");
    }

    request.tv_sec += getLong(argv[1], 0, "secs");
    request.tv_nsec += getLong(argv[2], 0, "nsecs");

    if (gettimeofday(&start, NULL) == -1) {
        errExit("gettimeofday");
    }
    
    for (;;) {
        s = clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &request, NULL);
        if (s == -1 && errno != EINTR) {
            errExit("clock_nanosleep");
        }
        
        if (gettimeofday(&finish, NULL) == -1) {
            errExit("gettimeofday");
        }
        
        printf("slept for: %9.6f sec\n", finish.tv_sec - start.tv_sec + 
                (finish.tv_usec - start.tv_usec) / 1000000.0);
        
        if (s == 0) {
            break;
        }
    }
    printf("sleep complete\n");
    exit(EXIT_SUCCESS);
}
