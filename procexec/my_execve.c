/*
 * my_execve.c
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <libgen.h>

extern char **environ;

int my_execlp(const char *filename, const char *arg, ...)
{
    int i; 
    int size = 2;
    char **argv;
    char *s;
    va_list ap;

    va_start(ap,arg);
    while ((s = va_arg(ap, char *)) != NULL) {
        size++;
    }
    va_end(ap);

    printf("%d\n", size);
    argv = malloc(sizeof(char *) * size);
    if (argv == NULL) {
        return -1;
    }

    argv[0] = basename((char *)filename);
    argv[1] = (char *)arg;

    va_start(ap, arg);
    for (i = 2; i < size+1; i++) {
        argv[i] = va_arg(ap, char *);
    }
    va_end(ap);

    return execve(filename, argv, environ);
}

int main()
{
    my_execlp("/usr/bin/cat","-n", "moo", (char *)NULL);
}

