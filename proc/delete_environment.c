/*
 * Checking on the glibc behavior 
 * of putenv.
 *
 * 16 November 2018
 */

#include <stdlib.h>
#include <stdio.h>

extern char **environ;

int
main()
{
    char **ep = NULL;
    
    printf("==BEFORE PUTENV==\n");
    for (ep = environ; *ep != NULL; ep++) {
        printf("%s\n", *ep);
    }
    
    putenv("MAIL");

    printf("\n\n==AFTER PUTENV==\n");
    for (ep = environ; *ep != NULL; ep++) {
        printf("%s\n", *ep);
    }


    exit(EXIT_SUCCESS);
}
