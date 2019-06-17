/*
 * become_daemon.h
 *
 * header file for become_daemon.c
 */

#ifndef BECOME_DAEMON_H
#define BECOME_DAEMON_H

/* bit mask values for 'flags' argument of becomeDaemon() */

#define BD_NO_CHDIR             01  /* don't chdir("/") */
#define BD_NO_CLOSE_FILES       02  /* don't close all open files */
#define BD_NO_REOPEN_STD_FDS    04  /* don't reopen stdin, stdout, and stderr 
                                       to /dev/null */

#define BD_NO_UMASK0           010  /* don't do a umask(0) */

#define BD_MAX_CLOSE           8192 /* maximum file descriptors to close if 
                                       sysconf(_SC_OPEN_MAX) is indeterminate */

int becomeDaemon(int flags);

#endif /* BECOME_DAEMON_H */
