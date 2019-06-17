/*
 * fifo_seqnum.h
 *
 * provided by TLPI
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <lib/tlpi_hdr.h>

#define SERVER_FIFO                 "/tmp/seqnum_sv"        /* well known server's FIFO */
#define CLIENT_FIFO_TEMPLATE        "/tmp/seqnum_cl.%ld"    /* template for building client FIFO name */
#define CLIENT_FIFO_NAME_LEN        (sizeof(CLIENT_FIFO_TEMPLATE) + 20)

struct request {        /* request (client --> server) */
    pid_t pid;          /* PID of client */
    int seq_len;        /* Length of desired sequence */
};

struct response {       /* response (server --> client) */
    int seq_num;        /* start of sequence */
};
