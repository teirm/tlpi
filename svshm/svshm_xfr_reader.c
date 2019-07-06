/*
 * svshm_xfr_reader.c
 *
 * Provided by TLPI.
 */

#include "svshm_xfr.h"

int
main(int argc, char *argv[])
{
    int semid;
    int shmid;
    int xfrs;
    int bytes;

    struct shmseg *shmp;

    semid = semget(SEM_KEY, 0, 0);
    if (semid == -1) {
        errExit("semget");
    }

    shmid = shmget(SHM_KEY, 0, 0);
    if (shmid == -1) {
        errExit("shmget");
    }

    shmp = shmat(shmid, NULL, SHM_RDONLY);
    if (shmp == (void *)-1) {
        errExit("shmat");
    }

    /* transfer blocks of data from shared memory to stdount */
    for (xfrs = 0, bytes = 0; ; xfrs++) {
        if (reserveSem(semid, READ_SEM) == -1) {
            errExit("reserveSem");
        }

        if (shmp->cnt == 0) {
            break;
        }
        bytes += shmp->cnt;

        if (write(STDOUT_FILENO, shmp->buf, shmp->cnt) != shmp->cnt) {
            fatal("partial/failed write");
        }

        if (releaseSem(semid, WRITE_SEM) == -1) {
            errExit("releaseSem");
        }
    }

    if (shmdt(shmp) == -1) {
        errExit("shmdt");
    }

    /* Give writer one more turn, so it can clean up */

    if (releaseSem(semid, WRITE_SEM) == -1) {
        errExit("releaseSem");
    }

    fprintf(stderr, "Received %d bytes (%d xfrs)\n", bytes, xfrs);
    exit(EXIT_SUCCESS);
}