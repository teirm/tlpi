/*
 * my_strerror.c
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include <lib/tlpi_hdr.h>

#include "my_strerror.h"

#define MAX_ERROR_LEN 256

static char buf[MAX_ERROR_LEN];

char *my_strerror(int err) 
{
    if (err < 0 || err >= sys_nerr || _sys_errlist[err] == NULL) {
        snprintf(buf, MAX_ERROR_LEN, "Unknown error %d", err);
    } else {
        strncpy(buf, _sys_errlist[err], MAX_ERROR_LEN - 1);
        buf[MAX_ERROR_LEN - 1] = '\0';
    }

    return buf;
}

static pthread_once_t once = PTHREAD_ONCE_INIT;
static pthread_key_t strerror_key;

static void my_strerror_tsd_destructor(void *buf) 
{
    free(buf);
}

static void my_strerror_tsd_create_key(void)
{
    int s;

    s = pthread_key_create(&strerror_key, my_strerror_tsd_destructor);
    if (s != 0) {
        errExitEN(s, "pthread_key_create");
    }
}

char *my_strerror_tsd(int err) 
{
    int s;
    char *buf;

    s = pthread_once(&once, my_strerror_tsd_create_key);
    if (s != 0) {
        errExitEN(s, "pthread_once");
    }

    buf = pthread_getspecific(strerror_key);
    if (buf == NULL) {
        buf = malloc(MAX_ERROR_LEN);
        if (buf == NULL) {
            errExit("malloc");
        }

        s = pthread_setspecific(strerror_key, buf);
        if (s != 0) {
            errExitEN(s, "pthread_setspecific");
        }
    }

    if (err < 0 || err >= _sys_nerr || _sys_errlist[err] == NULL) {
        snprintf(buf, MAX_ERROR_LEN, "Unknown error %d", err);
    } else {
        strncpy(buf, _sys_errlist[err], MAX_ERROR_LEN - 1);
        buf[MAX_ERROR_LEN - 1] = '\0';
    }

    return buf;
}

