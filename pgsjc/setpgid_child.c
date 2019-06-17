/*
 * setpgid_child.c
 *
 * Verify that process group can be changed
 * in a child if it does not exec.
 * 
 * 03 March 2019
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

int main(int argc, char *argv[])
{
    int rc;
    int i;
    pid_t child;
    
    switch (child = fork()) {
    case -1:
        /* error */
        fprintf(stderr, "ERROR: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    case 0:
        /* child */
        if (argc > 1) {
            rc = execv("./print_pgrp", argv);
            if (rc) {
                fprintf(stderr, "ERROR: %s\n", strerror(errno));
                _exit(EXIT_FAILURE);
            }
        }
        for (i = 0; i < 20; i++) {
            printf("child process group: %ld\n", (long)getpgrp());
            sleep(3);
        }
        _exit(EXIT_SUCCESS);
    default:
        /* parent */
        sleep(10);

        rc = setpgid(child, child);
        if (rc) {
            fprintf(stderr, "ERROR: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        exit(EXIT_SUCCESS);
    }
}
