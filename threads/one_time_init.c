/*
 * one_time_init.c
 *
 * Implementing a simplified version of pthread_once.
 *
 */

#include <stdio.h>
#include <pthread.h>

typedef struct one_time_s {
    int is_initialized;
    int foobar;
    pthread_mutex_t mtx;
} one_time_t;

static one_time_t control = {.is_initialized = 0, 
                             .mtx = PTHREAD_MUTEX_INITIALIZER};

void one_time_init(one_time_t *control,  void (*init)(void)) 
{
    pthread_mutex_lock(&control->mtx);
    if (!control->is_initialized) {
        init();
        control->is_initialized = 1;
    }
    pthread_mutex_unlock(&control->mtx);
}

static void test_function()
{
    printf("Running\n");
}

int main()
{
    one_time_init(&control, test_function);
    one_time_init(&control, test_function);
}
