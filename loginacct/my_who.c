/*
 * my_who.c
 *
 * Basic implementation of who(1)
 *
 * 4 April 2019
 */

#define _GNU_SOURCE
#include <time.h>
#include <utmpx.h>
#include <paths.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int opt; 
    int all_flag = 0;
    struct utmpx *ut;

    setutxent();

    while ((opt = getopt(argc, argv, "a")) != -1) {
        switch (opt) {
        case 'a': 
            all_flag = 1; 
            break;
        case '?': 
            /* FALLTHROUGH */ 
        default:
                  exit(EXIT_FAILURE);
        }
    }

    while ((ut = getutxent()) != NULL) {
        if (ut->ut_type == USER_PROCESS || (all_flag  && ut->ut_type == LOGIN_PROCESS)) {
            printf("%s\t", ut->ut_user);
            printf("%s\t", ut->ut_line);
            printf("%s", ctime((time_t *) &(ut->ut_tv.tv_sec)));
        }
    }
    endutxent();
    exit(EXIT_SUCCESS);
}
