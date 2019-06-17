/*
 * rusage.c
 *
 * displace resource usage of a command
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <sys/resource.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    int rc;
    struct rusage res_usage;
    pid_t child_pid;

    switch (child_pid = fork()) {
    case -1:
        /* error */
        fprintf(stderr, "ERROR: failed to execute %s -- %s\n", argv[1], strerror(errno));
        exit(EXIT_FAILURE);
    case 0:
        /* child */
        rc = execv(argv[1], argv + 1);
        if (rc) {
            fprintf(stderr, "ERROR: failed to execute %s -- %s\n", argv[1], strerror(errno));
            exit(EXIT_FAILURE);
        }
    default:
        
        rc = waitpid(child_pid, NULL, 0);
        if (rc == -1) {
            fprintf(stderr, "ERROR: failed to wait for child %s -- %s\n", argv[1], strerror(errno));
            exit(EXIT_FAILURE);
        }

        rc = getrusage(RUSAGE_CHILDREN, &res_usage);
        if (rc) {
            fprintf(stderr, "ERROR: failed to get resouce data for %s -- %s\n", argv[1], strerror(errno));
            exit(EXIT_FAILURE);
        }
        printf("system usecs used: %ld\n", (long)res_usage.ru_stime.tv_usec);
    }
    exit(EXIT_SUCCESS);
}
