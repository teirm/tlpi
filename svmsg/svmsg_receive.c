/*
 * svmsg_receive.c
 *
 * Provided by TLPI
 *
 * 04 June 2019
 */

#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/msg.h>
#include <lib/tlpi_hdr.h>

#define MAX_MTEXT 1024

struct mbuf {
    long mtype;
    char mtext[MAX_MTEXT];
};

static void
usageError(const char *prog_name, const char *msg)
{
    if (msg != NULL) {
        fprintf(stderr, "%s", msg);
    }
    
    fprintf(stderr, "Usage: %s [options] msqid [max-bytes]\n", prog_name);
    fprintf(stderr, "Permitted options are:\n");
    fprintf(stderr, "    -e       Use MSG_NOERROR flag\n");
    fprintf(stderr, "    -t type  Select message of a given type\n");
    fprintf(stderr, "    -n       Use IPC_NOWAIT flag\n");
#ifdef MSG_EXCEPT
    fprintf(stderr, "    -x       Use MSG_EXCEPT flag\n");
#endif /* MSG_EXCEPT */
    exit(EXIT_FAILURE);
}

int
main(int argc, char *argv[])
{
    int msqid;
    int flags;
    int type;

    ssize_t msg_len;
    size_t max_bytes;
    struct mbuf msg;
    
    int opt;

    /* parse command line */

    flags = 0;
    type = 0;
    while ((opt = getopt(argc, argv, "ent:x")) != -1) {
        switch (opt) {
            case 'e':       
                flags |= MSG_NOERROR; 
                break;
            case 'n':       
                flags |= IPC_NOWAIT; 
                break;
            case 't':       
                type = atoi(optarg); 
                break;
#ifdef MSG_EXCEPT
            case 'x':
                flags |= MSG_EXCEPT;
                break;
#endif /* MSG_EXCEPT */
            default:
                usageError(argv[0], NULL);
                break;
        }
    }

    if (argc < optind + 1 || argc > optind + 2) { 
        usageError(argv[0], "Wrong number of arguments\n");
    }

    msqid = getInt(argv[optind], 0, "msqid");
    max_bytes = (argc > optind + 1) ? 
                getInt(argv[optind + 1], 0, "max-bytes") : MAX_MTEXT;

    /* get message and display on stdout */

    msg_len = msgrcv(msqid, &msg, max_bytes, type, flags);
    if (msg_len == -1) 
        errExit("msgrcv");

    printf("Received: type=%ld; length=%ld", msg.mtype, (long)msg_len);
    if (msg_len > 0)
        printf("; body=%s", msg.mtext);
    printf("\n");

    exit(EXIT_SUCCESS);
}
