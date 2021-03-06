/*
 * sig_sender.c
 *
 * Provided by TLPI
 */

#include <signal.h>

#include <lib/tlpi_hdr.h>

int
main(int argc, char *argv[])
{
    int num_sigs;
    int sig;
    int j;

    pid_t pid;

    if (argc < 4 || strcmp(argv[1], "--help") == 0) {
        usageErr("%s pid num-sigs sig-num [sig-num-2]\n", argv[0]);
    }

    pid = getLong(argv[1], 0, "PID");
    num_sigs = getInt(argv[2], GN_GT_0, "num-sigs");

    /* send signals to receiver */

    printf("%s: sending signal %d to process %ld %d times\n",
            argv[0], sig, (long)pid, num_sigs);

    for (j = 0; j < num_sigs; j++) {
        if (kill(pid, sig) == -1)
            errExit("kill");
    }

    /* if a fourth argument was specified send that signal */

    if (argc > 4) {
        if (kill(pid, getInt(argv[4], 0, "sig-num-2")) == -1)
            errExit("kill");
    }

    printf("%s: exit\n", argv[0]);
    exit(EXIT_SUCCESS);
}
