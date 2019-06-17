/*
 * print_pgrp.c 
 *
 * A simple program that will print out the 
 * process group 20 times.
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
    int i;

    for (i = 0; i < 20; i++) {
        printf("process group: %ld\n", (long)getpgrp());
        sleep(3);
    }
    exit(EXIT_SUCCESS);
}
