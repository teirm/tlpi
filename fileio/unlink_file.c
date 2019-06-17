/*
 * Small c program to delete a grabage file.
 *
 * 07 November 2018
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

int main()
{
    int rc = 0;

    rc = unlink("./-a");

    if (rc != 0) {
        perror("Error");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}

