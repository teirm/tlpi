#include <syslog.h>

#include "svmsg_file.h"

#define MSG_SND_TIMEOUT 60

static int serverId; 

static void
grimReaper(int sig) 
{
    int savedErrno;

    savedErrno = errno;
    while (waitpid(-1, NULL, WNOHANG) > 0) {
        continue;
    }
    errno = savedErrno;
}

void
deleteServerFile()
{
    printf("Executing unlink\n");
    if (unlink(SERVER_ID_FILE) != 0) {
        fprintf(stderr, "Unable to delete server_id_file: %s\n", strerror(errno));
    }
}

void
handleCleanup(int sig)
{
    if (msgctl(serverId, IPC_RMID, NULL) == -1) {
        syslog(LOG_ERR, "Failed to remove msg_queue\n");
    }

    if (unlink(SERVER_ID_FILE) == -1) {
        syslog(LOG_ERR, "Failed to remove server id file\n");
    }

    signal(sig, SIG_DFL);
    raise(sig);
}

void
alarmHandler(int sig)
{
}


static void 
serveRequest(const struct requestMsg *req)
{
    int rc;
    int fd;
    int saved_errno; 
    ssize_t numRead;
    struct responseMsg resp;
    struct sigaction alarm_sa;

    alarm_sa.sa_flags = 0;
    sigemptyset(&alarm_sa.sa_mask);
    alarm_sa.sa_handler = alarmHandler;

    if (sigaction(SIGALRM, &alarm_sa, NULL) == -1) {
        errExit("sigaction");
    }

    fd = open(req->pathname, O_RDONLY);
    if (fd == -1) {
        resp.mtype = RESP_MT_FAILURE;
        snprintf(resp.data, sizeof(resp.data), "%s", "Could not open");
        rc = msgsnd(req->clientId, &resp, strlen(resp.data) + 1, 0);
        if (rc == -1) {
            syslog(LOG_ERR, "Failed to send error message: %s\n", resp.data);
            exit(EXIT_FAILURE);
        }
        syslog(LOG_ERR, "Could not open %s\n", req->pathname);
        exit(EXIT_FAILURE);
    }

    /* transmit the file contents in messages with type RESP_MT_DATA.
     * We don't diagnose read() and msgsnd() errors since we can't 
     * notify the client. */

    resp.mtype = RESP_MT_DATA;
    while ((numRead = read(fd, resp.data, RESP_MSG_SIZE)) > 0) { 
        if (msgsnd(req->clientId, &resp, numRead, 0) == -1) {
            syslog(LOG_ERR, "Failed to send message: %s\n", strerror(errno));
            break;
        }
    }
    
    if (numRead == -1) {
        syslog(LOG_ERR, "Failed to read: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    alarm(MSG_SND_TIMEOUT);
    resp.mtype = RESP_MT_END;
    rc = msgsnd(req->clientId, &resp, 0, 0);
    saved_errno = errno;
    alarm(0);
    errno = saved_errno;
    if (rc == -1) {
        if (errno == EINTR) {
            syslog(LOG_ERR, "msgsnd timed out\n");
        } else {
            syslog(LOG_ERR, "Failed to send RESP_MT_END message\n");
        }
        exit(EXIT_FAILURE);
    }
}

static void
writeServerId(int serverId)
{
    FILE *server_file;

    server_file = fopen(SERVER_ID_FILE, "w");
    
    if (server_file == NULL) {
        errExit("Failed to open server file\n");
    }

    if (fprintf(server_file, "%d", serverId) < 0) {
        errExit("Failed to write server file\n");
    }

    if (fclose(server_file) == -1) {
        errExit("Failed to close server file\n");
    }

    return;
}

int
main(int argc, char *argv[])
{
    int rc; 
    struct requestMsg req;
    pid_t pid;
    ssize_t msgLen;
    struct sigaction sa;
    struct sigaction cleanup_sa; 

    openlog(NULL, LOG_PID, LOG_USER);

    rc = daemon(0, 0);
    if (rc == -1) {
        errExit("becomeDaemon");
    }
    
    /* create server message queue */
    serverId = msgget(IPC_PRIVATE, IPC_CREAT | IPC_EXCL | 
                            S_IRUSR | S_IWUSR | S_IWGRP);

    if (serverId == -1) {
        errExit("msgget");
    }
    
    writeServerId(serverId);

    rc = atexit(deleteServerFile);
    if (rc != 0) {
        errExit("atexit");
    }

    /* establish SIGCHILD handler to reap terminated children. */

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = grimReaper;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) 
        errExit("sigaction");

    sigemptyset(&cleanup_sa.sa_mask);
    cleanup_sa.sa_flags = SA_RESTART;
    cleanup_sa.sa_handler = handleCleanup;
    if (sigaction(SIGINT, &cleanup_sa, NULL) == -1)
        errExit("sigaction");
    if (sigaction(SIGTERM, &cleanup_sa, NULL) == -1) 
        errExit("sigaction");

    /* read requests, handle each in a separate child process */

    for (;;) {
        msgLen = msgrcv(serverId, &req, REQ_MSG_SIZE, 0, 0);
        if (msgLen == -1) {
            if (errno == EINTR) 
                continue;
            errMsg("msgrcv");
            break;
        }

        pid = fork();
        if (pid == -1) {
            errMsg("fork");
            break;
        }

        if (pid == 0) {
            serveRequest(&req);
            _exit(EXIT_SUCCESS);
        }

        /* parent loops to receive next client request */
    }

    /* if msgrcv() or fork() fails, remove server MQ and exit */

    if (msgctl(serverId, IPC_RMID, NULL) == -1)
        errExit("msgctl");
    exit(EXIT_SUCCESS);
}
