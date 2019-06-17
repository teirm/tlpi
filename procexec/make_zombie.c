/*
 * make_zombie.c
 */

#include <signal.h>
#include <libgen.h>
#include <lib/tlpi_hdr.h>

#define CMD_SIZE    200

void sigchld_handler(int sig)
{
    return;
}


int main(int argc, char *argv[])
{
    char command[CMD_SIZE];
    pid_t child_pid;
    
    sigset_t new_mask;
    sigset_t old_mask;
    struct sigaction sa;

    sigaddset(&old_mask, SIGCHLD);

    setbuf(stdout, NULL);

    printf("Parent PID=%ld\n", (long)getpid());
    
    sigprocmask(SIG_BLOCK, &new_mask, &old_mask);
    switch (child_pid = fork()) {
    case -1:
        errExit("fork");
    case 0:
        sigprocmask(SIG_SETMASK, &old_mask, NULL);
        /* child immediately exits to become a zombie */
        printf("child (PID=%ld) exiting\n", (long)getpid());
        _exit(EXIT_SUCCESS);
    default:
        sa.sa_flags = 0;
        sa.sa_handler = sigchld_handler;
        sigemptyset(&sa.sa_mask);
        if (sigaction(SIGCHLD, &sa, NULL) == -1) {
            errExit("sigaction");
        }

        /* parent */
        sigsuspend(&old_mask); 
        snprintf(command, CMD_SIZE, "ps | grep %s", basename(argv[0]));
        system(command);

        if (kill(child_pid, SIGKILL) == -1) {
            errMsg("kill");
        }
        sleep(3);
        printf("after sending SIGKILL to zombie (PID=%ld):\n", (long)child_pid);
        system(command);

        exit(EXIT_SUCCESS);
    }
}
