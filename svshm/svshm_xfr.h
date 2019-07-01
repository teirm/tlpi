/* 
 * header file for shared memory prod-con example
 * Provided by TLPI
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include <sys/shm.h>

#include <svsem/binary_sems.h>
#include <lib/tlpi_hdr.h>

#define SHM_KEY 0x1234
#define SEM_KEY 0x5678

#define OBJ_PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)

#define WRITE_SEM 0
#define READ_SEM 1

#ifndef BUF_SIZE
#define BUF_SIZE 1024
#endif 

struct shmseg {
    int cnt;
    char buf[BUF_SIZE];
};
