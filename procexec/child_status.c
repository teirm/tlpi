/*
 * child_status.c
 */

#include <sys/wait.h>

#include <procexec/print_wait_status.h>
#include <lib/tlpi_hdr.h>

#ifdef WCONTINUED
#define WAITPID_FLAGS   (WUNTRACED | WCONTINUED)
#else
#define WAITPID_FLAGS   (WUNTRACED)
#endif /* WCONTINUED */


int main(int argc, char *argv[])
{
    int status;
    pid_t child_pid;
#ifdef USE_WAITID
    siginfo_t wait_info;
#endif /* USE_WAITID */

    if (argc > 1 && strcmp(argv[1], "--help") == 0) {
        usageErr("%s [exit-status]\n", argv[0]);
    }

    switch (fork()) {
    case -1: 
        errExit("fork");
    case 0:
        /* 
         * child -- either exits immediately with
         * status or loops waiting for signals.
         */
        printf("child started with pid = %ld\n", (long)getpid());
        if (argc > 1) {
            exit(getInt(argv[1], 0, "exit-status"));
        } else {
            for (;;) {
                pause();
            }
        }
        exit(EXIT_FAILURE);
    default:
        /* parent -- repeatedly wait on child until it either
         * exits or is terminated by a signal.
         */
        for (;;) {
#ifdef USE_WAITID
            child_pid = waitid(P_ALL, 0, &wait_info, WEXITED | WCONTINUED);
            if (child_pid == -1) {
                errExit("waitpid");
            }
            printf("waitid() returned: PID=%ld, status=0x%04x\n", 
                    (long)wait_info.si_pid, wait_info.si_status);
            printWaitIdStatus(NULL, &wait_info);
#else
            child_pid = waitpid(-1, &status, WAITPID_FLAGS);
            if (child_pid == -1) {
                errExit("waitpid");
            }
            printf("waitpid() returned: PID=%ld, status=0x%04x (%d, %d)\n",
                    (long) child_pid,
                    (unsigned int) status, status >> 8 , status & 0xff);
            printWaitStatus(NULL, status);
#endif /* USE_WAITID */


            if (WIFEXITED(status) || WIFSIGNALED(status)) 
                exit(EXIT_SUCCESS);
        }
    }
}


