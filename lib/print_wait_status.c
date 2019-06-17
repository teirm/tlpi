#define _GNU_SOURCE
#include <string.h>
#include <sys/wait.h>
#include <procexec/print_wait_status.h>
#include <lib/tlpi_hdr.h>

void printWaitStatus(const char *msg, int status)
{
    if (msg != NULL) {
        printf("%s", msg);
    }

    if (WIFEXITED(status)) {
        printf("child exited, status=%d\n", WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
        printf("child killed by signal %d (%s)",
                WTERMSIG(status), strsignal(WTERMSIG(status)));
#ifdef WCOREDUMP
        if (WCOREDUMP(status)) 
            printf(" (core dumped)");
#endif 
        printf("\n");
    } else if (WIFSTOPPED(status)) {
        printf("child stopped by signal %d (%s)", 
                WSTOPSIG(status), strsignal(WSTOPSIG(status)));
#ifdef WIFCONTINUED
    } else if (WIFCONTINUED(status)) {
        printf("child continued\n");
#endif
    } else {
        printf("what happend to this child? (status=%x)\n",
                (unsigned int)status);
    }
}


void printWaitIdStatus(const char *msg, siginfo_t *wait_info)
{
    int code;  
    int status;
    pid_t pid;

    if (msg != NULL) {
        printf("%s", msg);
    }

    if (wait_info == NULL) {
        return;
    }

    code = wait_info->si_code;
    status = wait_info->si_status;
    pid = wait_info->si_pid;

    switch (code) {
    case CLD_EXITED:
        printf("pid %ld terminated with _exit(%d)\n", (long)pid, status);
        break;
    case CLD_KILLED:
        printf("pid %ld killed with %d\n", (long)pid, status);
        break;
    case CLD_STOPPED:
        printf("pid %ld stopped with %d\n", (long)pid, status);
        break;
    case CLD_CONTINUED:
        printf("pid %ld continued with %d\n", (long)pid, status);
        break;
    default:
        printf("pid %ld unknown wait result %d\n", (long)pid, status);
        break;
    }
}
