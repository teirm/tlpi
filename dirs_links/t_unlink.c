/*
 * t_unlink.c
 *
 * Provided TLPI
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#include <sys/stat.h>

#include <lib/tlpi_hdr.h>

#define CMD_SIZE 256
#define BUF_SIZE 1024

int 
main(int argc, char *argv[])
{
    int fd;
    int j;
    int num_blocks;
    
    char shell_cmd[CMD_SIZE];
    char buf[BUF_SIZE];

    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        usageErr("%s temp-file [num-1k-blocks] \n", argv[0]);
    }

    num_blocks = (argc > 2) ? getInt(argv[2], GN_GT_0, "num-1k-blocks") 
                            : 10000;

    fd = open(argv[1], O_WRONLY | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        errExit("open");
    }

    if (unlink(argv[1]) == -1) {
        errExit("unlink");
    }

    for (j = 0; j < num_blocks; j++) {
        if (write(fd, buf, BUF_SIZE) != BUF_SIZE) {
            fatal("partial/failed write");
        }
    }

    snprintf(shell_cmd, CMD_SIZE, "df -k `dirname %s`", argv[1]);
    system(shell_cmd);

    if (close(fd) == -1) {
        errExit("close");
    }

    printf("********* closed file descriptor\n");

    system(shell_cmd);
    exit(EXIT_SUCCESS);
}
