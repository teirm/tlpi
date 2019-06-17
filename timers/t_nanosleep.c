/*
 * t_nanosleep.c
 *
 * Provided by TLPI
 */

#define _POSIX_C_SOURCE 199309
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
    struct timespec remain;

    struct sigaction sa;
    int s;

    if (argc != 3 || strcmp(argv[1], "--help") == 0) {
        usageErr("%s secs nanosecs\n", argv[0]);
    }

    request.tv_sec = getLong(argv[1], 0, "secs");
    request.tv_nsec = getLong(argv[2], 0, "nsecs");

    /* allow sigint to interrupt nanosleep */
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = sigint_handler;
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        errExit("sigaction");
    }
    if (gettimeofday(&start, NULL) == -1) {
        errExit("gettimeofday");
    }
    for (;;) {
        s = nanosleep(&request, &remain);
        if (s == -1 && errno != EINTR) {
            errExit("nanosleep");
        }

        if (gettimeofday(&finish, NULL) == -1) {
            errExit("gettimeofday");
        }
        
        printf("slept for: %9.6f secs\n", finish.tv_sec - start.tv_sec +
                (finish.tv_usec - start.tv_usec) / 1000000.0);

        if (s == 0) {
            break;
        }

        printf("Remaining: %2ld.%09ld\n", (long)remain.tv_sec, remain.tv_nsec);
        request = remain;
    }
    printf("sleep complete\n");
    exit(EXIT_SUCCESS);
}

