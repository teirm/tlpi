/*
 * t_sigqueue.c
 *
 * Provided by TLPI
 */

#define _POSIX_C_SOURCE 199309

#include <signal.h>
#include <lib/tlpi_hdr.h>

int
main(int argc, char *argv[])
{
    int sig;
    int num_sigs;
    int j;
    int sig_data;
    union sigval sv;

    if (argc < 4 || strcmp(argv[1], "--help") == 0)
        usageErr("%s pid sig-num data [num-sigs]\n", argv[0]);

    /* display pid and uin so they can be compared with fields of
     * siginfo_t argument supplied to handler in receiving
     * process.
     */

    printf("%s: PID is %ld, UID is %ld\n", argv[0], (long)getpid(), (long)getuid());

    sig = getInt(argv[2], 0, "sig-num");
    sig_data = getInt(argv[3], GN_ANY_BASE, "data");
    num_sigs = (argc > 4) ? getInt(argv[4], GN_GT_0, "num-sigs") : 1;

    for (j = 0; j < num_sigs; j++) {
        sv.sival_int = sig_data + j;
        if (sigqueue(getLong(argv[1], 0, "pid"), sig, sv) == -1)
            errExit("sigqueue %d", j);
    }

    exit(EXIT_SUCCESS);
}
