/*
 * ftok_verification.c
 *
 * Verifying the ftok algorithm.
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sysmacros.h>

int main()
{
    int key;
    int my_key;
    struct stat st_buf;
    char path_buf[PATH_MAX];

    if (getcwd(path_buf, PATH_MAX) == NULL) {
        fprintf(stderr, "ERROR: getwd() -- %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    
    if (stat(path_buf, &st_buf) == -1) {
        fprintf(stderr, "ERROR: stat() -- %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    
    key = ftok(path_buf, 1);
    if (key == -1) {
        fprintf(stderr, "ERROR: ftok -- %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    printf("ftok: 0x%x\n", key);
    printf("minor dev: 0x%x\nmajor dev: 0x%x\ninode number: 0x%lX\nproj: 0x%x\n",
            minor(st_buf.st_dev), major(st_buf.st_dev), st_buf.st_ino, 1);

    my_key = (1 & 0xff) << 24;
    my_key = my_key | (((minor(st_buf.st_dev)) & 0xff) << 16);
    my_key = my_key | (st_buf.st_ino & 0xffff);

    printf("my_key: 0x%x\n", my_key);



    exit(EXIT_SUCCESS);
}
