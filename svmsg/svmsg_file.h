/*
 * svmsg_file.h
 *
 * common header file for svmsg client
 * server.
 *
 * provided by Tlpi
 */

#include <sys/types.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <stddef.h>
#include <limits.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include <lib/tlpi_hdr.h>

#define SERVER_ID_FILE "/tmp/svmsg_file_id"

struct requestMsg {
    long mtype;
    int clientId;
    char pathname[PATH_MAX];
};

/* REQ_MSG_SIZE computes size of 'mtext' part of 'requestMsg' structure.
 * We use offsetof() to handle the possibility that there are padding
 * bytes between the 'clientId' and 'pathname' fields */

#define REQ_MSG_SIZE (offsetof(struct requestMsg, pathname) - \
                      offsetof(struct requestMsg, clientId) + PATH_MAX)

#define RESP_MSG_SIZE   8192

struct responseMsg {
    long mtype;
    char data[RESP_MSG_SIZE];
};

/* Types for response messages sent from server to client */
#define RESP_MT_FAILURE     1   /* File could not be opened */
#define RESP_MT_DATA        2   /* Message contains file data */
#define RESP_MT_END         3   /* File data complete */
