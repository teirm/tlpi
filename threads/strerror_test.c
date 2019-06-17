/*
 * strerror_test.c
 */

#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include <lib/tlpi_hdr.h>

#include "my_strerror.h"

static void *thread_func(void *arg)
{
    char *str;

    printf("Other thread about to call strerror()\n");
#if TSD 
    str = my_strerror_tsd(EPERM);
#else 
    str = my_strerror(EPERM);
#endif /* TSD */
    printf("Other thread: str (%p) = %s\n", str, str);

    return NULL;
}

int main(int argc, char *argv[])
{
    pthread_t t;
    int s;
    char *str;

#ifdef TSD 
    str = my_strerror_tsd(EINVAL);
#else 
    str = my_strerror(EINVAL);
#endif /* TSD */

    printf("Main thread has called strerror()\n");

    s = pthread_create(&t, NULL, thread_func, NULL);
    if (s != 0) {
        errExitEN(s, "pthread_create");
    }

    s = pthread_join(t, NULL);
    if (s != 0) {
        errExitEN(s, "pthread_join");
    }

    printf("Main thread: str (%p) = %s\n", str, str);

    exit(EXIT_SUCCESS);
}
