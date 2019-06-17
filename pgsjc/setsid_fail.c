/*
 * setsid_fail.c
 *
 * Verify that setsid will fail when called from the 
 * process group leader.
 * 
 * 03 March 2019  
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>

int main()
{
    pid_t new_sid;

    new_sid = setsid();
    if (new_sid == -1) {
        printf("ERROR: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    printf("Created new session\n");
    return 0;
}
