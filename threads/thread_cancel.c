/*
 * thread_cancel.c
 */

#include <pthread.h>
#include <lib/tlpi_hdr.h>

static void *thread_func(void *arg)
{
    int j;
    printf("new thread started\n");
    for (j = 1; ; j++)  {
        printf("loop %d\n", j);
        sleep(1);
    }

    /* NOTREACHED */
    return NULL;
}

int main(int argc, char *argv[])
{
    pthread_t thr;
    int s;
    void *res;

    s = pthread_create(&thr, NULL, thread_func, NULL);
    if (s != 0) {
        errExitEN(s, "pthread_create");
    }

    sleep(3);

    s = pthread_cancel(thr);
    if (s != 0) {
        errExitEN(s, "pthread_join");
    }

    if (res == PTHREAD_CANCELED) {
        printf("Thread was canceled\n");
    } else {
        printf("Thread was not canceled (should not happen!)\n");
    }

    exit(EXIT_SUCCESS);
}
