/*
 * svmsg_demo_server.c
 */

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>

#include <lib/tlpi_hdr.h>

#define KEY_FILE "/some-path/some-file"

int
main(int argc, char *argv[])
{
    int msq_id;
    key_t key;

    /* rw--w----*/
    const int MQ_PERMS = S_IRUSR | S_IWUSR | S_IWGRP;

    /* optional check to see if another server is running */

    key = ftok(KEY_FILE, 1);
    if (key == -1) {
        errExit("ftok");
    }

    /* while msgget() fails, try creating the queue exclusively */
    while ((msq_id = msgget(key, IPC_CREAT | IPC_EXCL | MQ_PERMS)) == -1) {
        if (errno == EEXIST) {
            msq_id = msgget(key, 0);
            if (msq_id == -1) {
                errExit("msgget() failed to retrieve old queue ID");
            }
            if (msgctl(msq_id, IPC_RMID, NULL) == -1) {
                errExit("msgget() failed to delete old queue");
            }
            printf("Removed old message queue (id=%d)\n", msq_id);
        } else {
            errExit("msgget() failed");
        }
    }

    /* upon loop exist, we've successfully created the message queue,
     * and we can then carry on to do other workd... */

    exit(EXIT_SUCCESS);
}
