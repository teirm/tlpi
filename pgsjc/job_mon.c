/*
 * job_mon.c 
 *
 * Provided by TLPI
 */

#define _GNU_SOURCE
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <lib/tlpi_hdr.h>

static int cmd_num;

static void handler(int sig)
{
    if (getpid() == getpgrp())
        fprintf(stderr, "Terminal FG process group: %ld\n",
                (long)tcgetpgrp(STDERR_FILENO));
    fprintf(stderr, "Process %ld (%d) received signal %d (%s)\n",
            (long)getpid(), cmd_num, sig, strsignal(sig));

    if (sig == SIGTSTP)
        raise(SIGSTOP);
}

int main(int argc, char *argv[])
{
    struct sigaction sa;

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = handler;
    if (sigaction(SIGINT, &sa, NULL) == -1)
        errExit("sigaction");
    if (sigaction(SIGTSTP, &sa, NULL) == -1) 
        errExit("sigaction");
    if (sigaction(SIGCONT, &sa, NULL) == -1)
        errExit("sigaction");

    /* if stdin is a terminal, this is the first process in pipeline:
     * print a heading an initialize message to be sent down pipe */

    if (isatty(STDIN_FILENO)) {
        fprintf(stderr, "Terminal FG process group: %ld\n",
                (long) tcgetpgrp(STDIN_FILENO));
        fprintf(stderr, "Command   PID  PPID  PGRP  SID\n");
        cmd_num = 0;
    } else {
        if (read(STDIN_FILENO, &cmd_num, sizeof(cmd_num)) <= 0)
            fatal("read got EOF or error");
    }
    
    cmd_num++;
    fprintf(stderr, "%4d    %5ld %5ld %5ld %5ld\n", cmd_num,
            (long)getpid(), (long)getppid(), (long)getpgrp(), (long)getsid(0));
    
    if (!isatty(STDOUT_FILENO))
        if (write(STDOUT_FILENO, &cmd_num, sizeof(cmd_num)) == -1)
            errMsg("write");

    for (;;)
        pause();
}

