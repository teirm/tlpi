/*
 * get_child_usage
 *
 * Create a child process but show that 
 * the resource limit is only modified
 * after a call to wait has been made.
 *
 * 08 March 2019
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
    int i;
    pid_t child_pid;
    struct rusage res_usage;

    switch (child_pid = fork()) {
    case -1:
        /* error */
        fprintf(stderr, "ERROR: failure to fork child process -- %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    case 0:
        /* child */
        for (i = 0; i < 10000; i++) {
        }
        exit(EXIT_SUCCESS);
    default:
        /* parent */
        sleep(3);
        rc = getrusage(RUSAGE_CHILDREN, &res_usage);
        if (rc) {
            fprintf(stderr, "ERROR: failure to get RUSAGE_CHILDREN -- %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        printf("CPU time consumed by children before waitpid: %ld\n", res_usage.ru_utime.tv_usec);

        rc = waitpid(0, NULL, 0);
        if (rc == -1) {
            fprintf(stderr, "ERROR: failure to waitpid -- %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        rc = getrusage(RUSAGE_CHILDREN, &res_usage);
        if (rc) {
            fprintf(stderr, "ERROR: failure to get RUSAGE_CHILDREN -- %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        printf("CPU time consumed by children after waitpid: %ld\n", res_usage.ru_utime.tv_usec);
    }

}
