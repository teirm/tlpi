/*
 * my_getlogin.c
 *
 * An implementation of getlogin().
 */

#define _GNU_SOURCE
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <utmpx.h>
#include <string.h>
#include <errno.h>

char *getlogin()
{
    int found = 0;
    struct utmpx *ut = NULL; 
    char *dev_name = NULL;
    char *tty_abbrv = NULL;
    
    static char user_name[__UT_NAMESIZE];
    memset(user_name, 0, sizeof(user_name));

    dev_name = ttyname(STDIN_FILENO); 
    if (dev_name == NULL) {
        fprintf(stderr, "ERROR: ttyname -- %s\n", strerror(errno));
        return NULL; 
    }
    /* the dev_name comes out as /dev/tty[n]
     * but ut_line will only have the tty[n] 
     * part. Add 5 to move get only the tty[n]
     * portion.
     */
    tty_abbrv = dev_name + 5;

    setutxent();

    while ((ut = getutxent()) != NULL) {
        if (strcmp(tty_abbrv, ut->ut_line) == 0) {
           memcpy(user_name, ut->ut_user, __UT_NAMESIZE);
           found = 1;
           break;
        } 
    }

    endutxent();
    return (found ? user_name : NULL); 
}

int main()
{
    printf("%s\n", getlogin());
}
