/*
 * fifo_seqnum_client.c
 */

#include "fifo_seqnum.h"

static char client_fifo[CLIENT_FIFO_NAME_LEN];

static void remove_fifo(void)
{
    unlink(client_fifo);
}

int main(int argc, char *argv[])
{
    int server_fd;
    int client_fd;
    
    struct request req;
    struct response resp;

    if (argc > 1 && strcmp(argv[1], "--help") == 0) {
        usageErr("%s [seq-len...]\n", argv[0]);
    }

    /* create our FIFO (before sending request, to avoid a race) */

    umask(0);
    snprintf(client_fifo, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMPLATE,
            (long)getpid());
    if (mkfifo(client_fifo, S_IRUSR | S_IWUSR | S_IWGRP) == -1
            && errno != EEXIST)
        errExit("mkfifo %s", client_fifo);

    if (atexit(remove_fifo) != 0)
        errExit("atexit");

    /* construct request message, open server FIFO, and send request */

    req.pid = getpid();
    req.seq_len = (argc > 1) ? getInt(argv[1], GN_GT_0, "seq-len") : 1;

    server_fd = open(SERVER_FIFO, O_WRONLY);
    if (server_fd == -1)
        errExit("open %s", SERVER_FIFO);

    if (write(server_fd, &req, sizeof(struct request)) != sizeof(struct request))
        fatal("Can't write to server");

    /* open our FIFO, read and display resposne */
    client_fd = open(client_fifo, O_RDONLY);
    if (client_fd == -1)
        errExit("open %s", client_fifo);

    if (read(client_fd, &resp, sizeof(struct response)) != sizeof(struct response))
        fatal("Can't read response from server");

    printf("%d\n", resp.seq_num);
    exit(EXIT_SUCCESS);
}
