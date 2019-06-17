/*
 * my_nice.c
 *
 * An implementation of the nice(1)
 * command.
 *
 * 06 March 2019
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sched.h>
#include <string.h>
#include <errno.h>

#include <sys/resource.h>

int main(int argc, char *argv[])
{
    int rc; 
    int priority; 
    
    if (argc == 1) {
        priority = getpriority(PRIO_PROCESS, 0);
        if (priority == -1) {
            fprintf(stderr, "ERROR: unable to get current process priority -- %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        printf("%d\n", priority);
    } else {
        priority = atoi(argv[2]);
        if (priority > 19 || priority < -20) {
            fprintf(stderr, "invalid value for -n\n");
            exit(EXIT_FAILURE);
        }
        rc = setpriority(PRIO_PROCESS, 0, priority);
        if (rc) {
            fprintf(stderr, "ERROR: unable to set priority -- %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        execv(argv[3], argv + 3);
    } 
    exit(EXIT_SUCCESS);
}
