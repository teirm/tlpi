/*
 * thread_incr.c
 * 
 * Provided by TLPI
 */

#include <pthread.h>
#include <string.h>

#include <lib/tlpi_hdr.h>

static volatile int glob = 0;
typedef struct thread_args_s {
    int loops;
    int id;
} thread_args_t;

static void *thread_function(void *arg)
{
    thread_args_t args = (*(thread_args_t *)arg); 
    
    int loops = args.loops;
    int id = args.id;
    int loc;
    int j;

    for (j = 0; j < loops; j++) {
        loc = glob;
        loc++;
        glob = loc;
        printf("[thread id: %d] glob = %d\n", id, glob);
    }

    return NULL;
}

int main(int argc, char *argv[])
{
    int s;
    int loops;
    
    pthread_t t1;
    pthread_t t2;
    
    thread_args_t t1_args;
    thread_args_t t2_args;

    memset(&t1_args, 0, sizeof(thread_args_t));
    memset(&t2_args, 0, sizeof(thread_args_t));

    loops = (argc > 1) ? getInt(argv[1], GN_GT_0, "num-loops") : 1000000;

    t1_args.loops = loops;
    t2_args.loops = loops; 
    t1_args.id = 1;
    t1_args.id = 2;

    s = pthread_create(&t1, NULL, thread_function, &t1_args);
    if (s != 0) {
        errExitEN(s, "pthread_create");
    }
    
    s = pthread_create(&t2, NULL, thread_function, &t2_args);
    if (s != 0) {
        errExitEN(s, "pthread_create");
    }
    
    s = pthread_join(t1, NULL);
    if (s != 0) {
        errExitEN(s, "pthread_join");
    }

    s = pthread_join(t2, NULL);
    if (s != 0) {
        errExitEN(s, "pthread_join");
    }

    printf("glob = %d\n", glob);
    exit(EXIT_SUCCESS);
}
