/*
 * timed_read.c
 *
 * Provided by TLPI
 */

#include <signal.h>
#include <lib/tlpi_hdr.h>

#define BUF_SIZE 200

/* SIGALRM handler: interrupts blocked system call */
static void
handler(int sig)
{
    printf("caught signal\n");
}

int
main(int argc, char *argv[])
{
    int saved_errno;
    ssize_t num_read;
    
    struct sigaction sa;
    char buf[BUF_SIZE];

    if (argc > 1 && strcmp(argv[1], "--help") == 0) {
        usageErr("%s [num-secs [restart-flag]]\n", argv[0]);
    }

    /* 
     * setup sigalrm handler and allow system calls to be
     * interrupted unless a command-line arg was provided.
     */
    sa.sa_flags = (argc > 2) ? SA_RESTART : 0;
    sigemptyset(&sa.sa_mask);

    sa.sa_handler = handler;
    if (sigaction(SIGALRM, &sa, NULL) == -1) {
        errExit("sigaction");
    }

    alarm((argc > 1) ? getInt(argv[1], GN_NONNEG, "num-secs") : 10);

    num_read = read(STDIN_FILENO, buf, BUF_SIZE);

    saved_errno = errno;
    /* cancel any pending alarm request */
    alarm(0);
    errno = saved_errno;

    /* determine result of read() */
    if (num_read == -1) {
        if (errno == EINTR) 
            printf("Read timed out\n");
        else
            errMsg("read");
    } else {
        printf("Successful read (%ld bytes): %.*s", (long)num_read, (int)num_read, buf);
    }

    exit(EXIT_SUCCESS);
}
