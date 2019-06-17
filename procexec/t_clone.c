/*
 * t_clone.c
 */

#define _GNU_SOURCE
#include <signal.h>
#include <fcntl.h>
#include <sched.h>
#include <sys/wait.h>
#include <lib/tlpi_hdr.h>


/* 
 * signal to be generated on termination of 
 * cloned child.
 */
#ifndef CHILD_SIG
#define CHILD_SIG SIGUSR1   
#endif 

static int child_func(void *arg)
{
    if (close(*((int *) arg)) == -1) {
        errExit("close");
    }

    return 0;
}

int main(int argc, char *argv[])
{
    const int STACK_SIZE = 65536;
    char *stack;
    char *stack_top;
    int s;
    int fd;
    int flags;

    fd = open("/dev/null", O_RDWR);
    if (fd == -1) {
        errExit("open");
    }

    /* if argc > 1, child shares file descriptor table with parent */

    flags = (argc > 1) ? CLONE_FILES : 0;

    /* allocate stack for child */
    stack = malloc(STACK_SIZE);
    if (stack == NULL) {
        errExit("malloc");
    }
    stack_top = stack + STACK_SIZE;

    /* ignore CHILD_SIG, in case it is a signal whose default is to termiante
     * the process; but don't ignore SIGCHLD (which is ignored by default), since
     * that would prevent the creation of a zombie.
     */

    if (CHILD_SIG != 0 && CHILD_SIG != SIGCHLD) {
        if (signal(CHILD_SIG, SIG_IGN) == SIG_ERR) {
            errExit("signal");
        }
    }

    /* create child -- child commences execution in child_func */
    if (clone(child_func, stack_top, flags | CHILD_SIG, (void *)&fd) == -1) {
        errExit("clone");
    }

    /* parent falls thorugh to here -- wait for child; __WCLONE is needed for 
     * child notifying with signal other than SIGCHLD.
     */
    if (waitpid(-1, NULL, (CHILD_SIG != SIGCHLD) ? __WCLONE : 0) == -1) {
        errExit("waitpid");
    }
    printf("child has terminated\n");

    /* did close of file descriptor in child affect parent? */
    if (s == -1 && errno == EBADF) {
        printf("file descriptor %d has been closed\n", fd);
    } else if (s == -1) {
        printf("write() on file descriptor %d failed unexpectedly (%s)\n", 
                fd, strerror(errno));
    } else {
        printf("write() on file descriptor %d succeeded\n", fd);
    }

    exit(EXIT_SUCCESS);
}
