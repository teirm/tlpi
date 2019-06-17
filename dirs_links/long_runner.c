/*
 * longer_runner.c
 *
 * Small program that does nothing but loops and sleeps.
 */

#include <unistd.h>

int
main()
{
    for (;;)     
        sleep(5);
}
