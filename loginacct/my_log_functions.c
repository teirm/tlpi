/*
 * my_log_functions.c
 *
 * Implementations of login(3), logout(3), 
 * logwtmp(3).
 *
 */

#define _GNU_SOURCE
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <utmpx.h>
#include <paths.h>
#include <string.h>

void my_login(struct utmpx *ut)
{
    char *dev_name; 
    if (ut == NULL)
        return;

    ut->ut_type = USER_PROCESS;
    ut->ut_pid = getpid();
    
    dev_name = ttyname(STDIN_FILENO); 
    if (dev_name == NULL) {
        strncpy(ut->ut_line, "???", sizeof(ut->ut_line)); 
        return;
    }

    strncpy(ut->ut_line, dev_name + 5, sizeof(ut->ut_line));
    setutxent();
    if (pututxline(ut) == NULL) {
        return;
    }
    endutxent();

    return;
}

void my_logout(const char *ut_line)
{
    struct utmpx *ut = NULL;

    setutxent();

    while ((ut = getutxent()) != NULL) {
        if (strcmp(ut->ut_line, ut_line) == 0) {
            break;
        }
    }
    
    if (!ut) {
        return;
    }

    memset(ut->ut_user, 0, sizeof(ut->ut_user));
    memset(ut->ut_host, 0, sizeof(ut->ut_host));

    time((time_t *)&ut->ut_tv.tv_sec);
    ut->ut_type = DEAD_PROCESS;

    setutxent();
    if (pututxline(ut) == NULL) {
        endutxent();
        return;
    }

    endutxent();
    return;
}

void my_logwtmp(const char *line, const char *name, const char *host)
{
    struct utmpx ut;
   
    memset(&ut, 0, sizeof(ut));
    
    strncpy(ut.ut_line, line, sizeof(ut.ut_line));
    strncpy(ut.ut_user, name, sizeof(ut.ut_user));
    strncpy(ut.ut_host, host, sizeof(ut.ut_host));

    updwtmpx(_PATH_WTMP, &ut);

    return;
}
