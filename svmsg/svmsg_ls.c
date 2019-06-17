/*
 * svmsg_ls.c
 *
 * Provided TLPI
 */

#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/msg.h>
#include <lib/tlpi_hdr.h>

int
main(int argc, char *argv[])
{
    int maxind;
    int ind;
    int msqid;
    struct msqid_ds ds;
    struct msginfo msginfo;

    /* obtain size of kernel 'entries' array */
    maxind = msgctl(0, MSG_INFO, (struct msqid_ds *)&msginfo);
    if (maxind == -1) {
        errExit("msgclt-MSG_INFO");
    }

    printf("maxind: %d\n\n", maxind);
    printf("index     id    key    messages\n");

    /* Retrieve and display information from each element of 'entries' array */

    for (ind = 0; ind <= maxind; ind++) {
        msqid = msgctl(ind, MSG_STAT, &ds);
        if (msqid == -1) {
            if (errno != EINVAL && errno != EACCES) {
                errMsg("msgctl-MSG_STAT");
            }
            continue;
        }

        printf("%4d %8d  0x%08lx %7ld\n", ind, msqid,
                (unsigned long)ds.msg_perm.__key, (long)ds.msg_qnum);
    }
    exit(EXIT_SUCCESS);
}
