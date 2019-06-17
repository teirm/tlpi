/*
 * longjmp to a returned frame
 */

#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>

static jmp_buf env;

static void
f2(void)
{
    switch(setjmp(env)) {
        case 1:
            printf("Initial return from setjmp()\n");
            break;
        case 2:
            printf("Return from longjmp()\n");
            break;
        default:
            printf("Unexpected return\n");
            break;
    }
}

static void
f1(void)
{
    f2();
    longjmp(env, 1);
}


int main()
{
    f1();

    exit(EXIT_SUCCESS);
}
