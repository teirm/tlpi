/*
 * join_self.c 
 *
 * Seeing what happens if a thread joins itself.
 */

#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    pthread_t tid;
    int s;

    s = pthread_join(pthread_self(), NULL);
    if (s != 0) {
        printf("failed to join: %s\n", strerror(s));
    }
}
