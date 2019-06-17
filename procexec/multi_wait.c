/*
 * multi_wait.c
 *
 * provided TLPI.
 */

#include <time.h>

#include <sys/wait.h>

#include <time/curr_time.h>
#include <lib/tlpi_hdr.h>

int 
main(int argc, char *argv[])
{
    int num_dead;
    pid_t child_pid;
    int j;

    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        usageErr("%s sleep-time...\n", argv[0]);
    }

    setbuf(stdout, NULL);

    for (j = 1; j < argc; j++) {
        switch (fork()) {
            case -1:
                errExit("fork");
            case 0:
                printf("[%s] child %d started with pid %ld, sleeping %s seconds\n", 
                        currTime("%T"), j, (long)getpid(), argv[j]);
                sleep(getInt(argv[j], GN_NONNEG, "sleep-time"));
                _exit(EXIT_SUCCESS);
            default:
                break;
        }
    }

    num_dead = 0;
    for (;;) {
        child_pid = wait(NULL);
        if (child_pid == -1) {
            if (errno == ECHILD) {
                printf("no more children - bye!\n");
                exit(EXIT_SUCCESS);
            } else {
                errExit("wait");
            }
        }

        num_dead++;

        printf("[%s] wait() returned child PID %ld (num_dead=%d)\n",
                currTime("%T"), (long)child_pid, num_dead);
    }
}

