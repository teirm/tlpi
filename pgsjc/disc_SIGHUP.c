/*
 * disc_SIGHUP.c
 *
 * provided by TLPI
 */

#define _GNU_SOURCE
#include <string.h>
#include <signal.h>

#include <lib/tlpi_hdr.h>

static void handler(int sig)
{
    printf("PID %ld: caught signal %2d (%s)\n", 
            (long)getpid(), sig, strsignal(sig));
}

int main(int argc, char* argv[])
{
    pid_t parent_pid;
    pid_t child_pid;
    int j;
    struct sigaction sa;

    if (argc < 2 || strcmp(argv[1], "--help") == 0) 
        usageErr("%s {d|s}... [ > sig.log 2&>1 ]\n", argv[0]);

    setbuf(stdout, NULL);

#ifdef IGNORE_SIGHUP
    signal(SIGHUP, SIG_IGN);
#endif /* IGNORE_SIGHUP */


    parent_pid = getpid();
    printf("PID of parent process is:       %ld\n", (long)parent_pid);
    printf("Foreground process group ID is: %ld\n",
            (long)tcgetpgrp(STDIN_FILENO));

    for (j = 1; j < argc; j++) {
        child_pid = fork();
        if (child_pid == -1)
            errExit("fork");

        if (child_pid == 0) {               /* if child ... */
            if (argv[j][0] == 'd')          /* 'd' --> to different pgrp */
                if (setpgid(0, 0) == -1)
                    errExit("setpgid");

            sigemptyset(&sa.sa_mask);
            sa.sa_flags = 0;
            sa.sa_handler = handler;
            if (sigaction(SIGHUP, &sa, NULL) == -1) {
                errExit("sigaction");
            }
            break;
        }
    }

    /* all processes fall through to here */
    alarm(60);

    printf("PID=%ld PGID=%ld\n", (long)getpid, (long)getpgrp());
    for (;;)
        pause();
}
