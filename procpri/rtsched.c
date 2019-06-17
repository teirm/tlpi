/*
 * rtsched.c 
 *
 * A setuid-root program to run
 * a process at a given elevated scheduler
 * priority.
 *
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sched.h>
#include <errno.h>

int main(int argc, char *argv[])
{
    int rc;
    int sched_type;
    struct sched_param sp; 
    
    if (argc < 4 || strcmp(argv[1], "--help") == 0) {
        printf("./rtsched [r|f] priority comand arg...\n");
        exit(EXIT_SUCCESS);
    }
    
    if (argv[1][0] == 'r') {
        sched_type = SCHED_RR;
    } else if (argv[1][0] == 'f') {
        sched_type = SCHED_FIFO;
    } else {
        printf("invalid scheduler policy -- %c\n", argv[1][0]);
        exit(EXIT_FAILURE);
    }
    
    sp.sched_priority = atoi(argv[2]);

    rc = sched_setscheduler(0, sched_type, &sp);
    if (rc) {
        fprintf(stderr, "ERROR: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    
    setuid(1000);
    rc = execv(argv[3], argv + 3);
    if (rc) {
        fprintf(stderr, "ERROR: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

}
