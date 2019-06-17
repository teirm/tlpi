/*
 * fifo_sched.c
 *
 * Schedule parent and child in fifo real time
 */

#define _GNU_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <string.h>
#include <errno.h>

#include <sys/times.h>
#include <sys/types.h>


static void do_work()
{
    struct tms buf;
    clock_t start_time; 
    memset(&buf, 0, sizeof(struct tms)); 

    /* get start time */
    times(&buf);
    start_time = buf.tms_utime;
    for (;;) {
        times(&buf);
        
        if (buf.tms_utime % 25 == 0) {
            printf("pid %ld  total_cpu %lf\n", 
                    (long)getpid(), (double)buf.tms_utime / 100);
        }

        if ((buf.tms_utime /100) > 2) {
            printf("all done"); 
            break;
        }
    }
}


int main()
{
    int rc;
    cpu_set_t set;
    struct sched_param sp; 

    /* install alarm for 10 s  in case something goes wrong */
    alarm(30);    

    CPU_ZERO(&set);
    CPU_SET(1, &set);
    rc = sched_setaffinity(0, sizeof(cpu_set_t), &set);
    if (rc) {
        fprintf(stderr, "ERROR: failed to set scheduler affinity -- %s\n", strerror(errno));
        exit(EXIT_FAILURE); 
    }
    
    sp.sched_priority = sched_get_priority_max(SCHED_FIFO) - 1;
    rc = sched_setscheduler(0, SCHED_FIFO, &sp);
    if (rc) {
        fprintf(stderr, "ERROR: failed to set scheduler priority -- %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    switch (fork()) {
        case -1:
            /* error */
            fprintf(stderr, "ERROR: failed to fork child -- %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        
        case 0:
            /* child */
            do_work();
            break;
        default:
            /* parent */
            do_work();
            break;
    }

    exit(EXIT_SUCCESS);
}
