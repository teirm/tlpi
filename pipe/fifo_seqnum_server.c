/*
 * fifo_seqnum_server.c
 */

#include <signal.h>
#include <string.h>
#include <errno.h>
#include "fifo_seqnum.h"

#define BACKUP_FILE     "/tmp/fifo_seqnum_server_backup"

static int get_backup_file();
static int create_backup_file();
static int get_backup_seq(int backup_fd);
static int update_backup_seq(int seqnum, int backup_fd);

static void sig_handler(int sig);

static int server_fd;
static int dummy_fd;

static int get_backup_file()
{
    int backup_fd;
    backup_fd = open(BACKUP_FILE, O_RDWR | O_SYNC);

    return backup_fd;
}

static int create_backup_file()
{
    int backup_fd;
    backup_fd = open(BACKUP_FILE, O_RDWR | O_SYNC | O_CREAT);
    
    return backup_fd;
}

static int get_backup_seq(int backup_fd)
{
    int backup_seq;
    ssize_t num_read;

    num_read = read(backup_fd, &backup_seq, sizeof(int));
    if (num_read == 0) {
        return -1;
    }

    if (num_read == -1) {
        return -1;
    }

    return backup_seq;
}

static int update_backup_seq(int seqnum, int backup_fd)
{
    if (lseek(backup_fd, SEEK_SET, 0) == -1) {
        return -1;
    }
    
    if (write(backup_fd, &seqnum, sizeof(int)) != sizeof(int)) {
        return -1;
    }

    return 0;
}

static void sig_handler(int sig)
{
    int rc = 0;  
    close(server_fd);
    close(dummy_fd);
    unlink(SERVER_FIFO);
    _exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
    int client_fd;
    int backup_fd;
    int seq_num = 0;
    int rc = 0;

    char client_fifo[CLIENT_FIFO_NAME_LEN];

    struct request req;
    struct response resp;
    
    backup_fd = get_backup_file();
    if (backup_fd == -1 && errno == ENOENT) {
        backup_fd = create_backup_file();
        if (backup_fd == -1) {
            errExit("Failed to open or create backup file\n");
        }
    }

    if (backup_fd == -1) {
        errExit("Failed to open backup file\n");
    }
    
    seq_num = get_backup_seq(backup_fd);
    if (seq_num == -1) {
        seq_num = 0; 
    }
    /* create well-known FIFO, and open it for reading */
    
    if (signal(SIGINT, sig_handler) == SIG_ERR) {
        errExit("signal");
    }
    
    if (signal(SIGTERM, sig_handler) == SIG_ERR) {
        errExit("signal");
    }

    umask(0);
    if (mkfifo(SERVER_FIFO, S_IRUSR | S_IWUSR | S_IWGRP) == -1 && 
            errno != EEXIST) {
        errExit("mkfifo %s", SERVER_FIFO);
    }

    server_fd = open(SERVER_FIFO, O_RDONLY);
    if (server_fd == -1) {
        errExit("open %s", SERVER_FIFO);
    }

    /* open an extra write descriptor, so that we never see EOF */
    dummy_fd = open(SERVER_FIFO, O_WRONLY);
    if (dummy_fd == -1) {
        errExit("open %s", SERVER_FIFO);
    }

    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
        errExit("signal");
    }


    for (;;) {
        if (read(server_fd, &req, sizeof(struct request)) != sizeof(struct request)) {
            fprintf(stderr, "Error reading request; discarding\n");
            continue;
        }

        /* open client fifo (previously created by client) */

        snprintf(client_fifo, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMPLATE,
                (long) req.pid);
        client_fd = open(client_fifo, O_WRONLY | O_NONBLOCK);
        if (client_fd == -1) {
            errMsg("open %s", client_fifo);
            continue;
        }

        /* send response and close FIFO */
        resp.seq_num = seq_num;
        if (write(client_fd, &resp, sizeof(struct response)) 
                != sizeof(struct response)) {
            fprintf(stderr, "Error writing to FIFO %s\n", client_fifo);
        }
        
        if (close(client_fd) == -1) {
            errMsg("close");
        }

        seq_num += req.seq_len;
        
        rc = update_backup_seq(seq_num, backup_fd);
        if (rc == -1) {
            errMsg("failed to update backup");
        }
    }

    if (close(backup_fd) == -1) {
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}

