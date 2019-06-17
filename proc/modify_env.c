#define __GNU_SOURCE
#include <stdlib.h>
#include <lib/tlpi_hdr.h>

extern char **environ;

int
main(int argc, char *argv[])
{
    int j;
    char **ep;

    clearenv();         /* erase the entire environment */

    for (j = 1; j < argc; j++) {
        if (putenv(argv[j]) != 0) {
            errExit("putenv: %s", argv[j]);
        }
    }

    if (setenv("GREET", "Hello world", 0) == -1) {
        errExit("setenv");
    }

    unsetenv("BYE");

    for (ep = environ; *ep != 0; ep++) {
        puts(*ep);
    }

    exit(EXIT_SUCCESS);
}
