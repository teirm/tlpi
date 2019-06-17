/*
 * svmsg_send.c
 */

#include <sys/types.h>
#include <sys/msg.h>

#include <lib/tlpi_hdr.h>

#define MAX_MTEXT 1024

struct mbuf {
    long mtype;
    char mtext[MAX_MTEXT];
};

static void
usageError(const char *progName, const char *msg)
{
    if (msg != NULL) {
        fprintf(stderr, "%s", msg);
    }
    fprintf(stderr, "Usage: %s [-n] msqid msg-type [msg-text\n", progName);
    fprintf(stderr, "   -n          Use IPC_NOWAIT flag\n");
    exit(EXIT_FAILURE);
}

int
main(int argc, char *argv[])
{
    int msq_id;
    int flags;
    int msg_len;
    int opt;

    struct mbuf msg;

    /* parse command-line options and arguments */

    flags = 0;
    while ((opt = getopt(argc, argv, "n")) != -1) {
        if (opt == 'n')
            flags |= IPC_NOWAIT;
        else
            usageError(argv[0], NULL);
    }

    if (argc < optind + 2 || argc > optind + 3) {
        usageError(argv[0], "Wrong number of arguments\n");
    }

    msq_id = getInt(argv[optind], 0, "msqid");
    msg.mtype = getInt(argv[optind + 1], 0, "msg-type");

    if (argc > optind + 2) {
        msg_len = strlen(argv[optind + 2]) + 1;
        if (msg_len > MAX_MTEXT)
            cmdLineErr("msg-text too long (max: %d characters)\n", MAX_MTEXT);

        memcpy(msg.mtext, argv[optind + 2], msg_len);
    } else {
        msg_len = 0;
    }

    /* send message */
    if (msgsnd(msq_id, &msg, msg_len, flags) == -1) {
        errExit("msgsnd");
    }

    exit(EXIT_SUCCESS);
}
