/*
 * my_chattr.c
 *
 * A simplified version of the chattr
 * program.
 *
 * Does not support -R -V -v.
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include <sys/ioctl.h>

#include <linux/fs.h>

static inline void  
do_ioctl(int fd, unsigned long req, int *attr)
{
    if (ioctl(fd, req, attr) == -1) {
        fprintf(stderr, "ERROR: ioctl error - %s\n",
                strerror(errno));
        exit(EXIT_FAILURE);
    }
    
    return;
}


int
main(int argc, char *argv[])
{
    int i;
    int fd;
    int attr;
    int flag_count;

    if (argc < 3) {
        fprintf(stderr, "Usage: %s <flags> <file_name>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    if ((fd = open(argv[2], O_RDONLY | O_WRONLY)) == -1) {
        fprintf(stderr, "ERROR: Unable to open %s -- %s\n",
                argv[2], strerror(errno));
        exit(EXIT_FAILURE);
    }

    do_ioctl(fd, FS_IOC_GETFLAGS, &attr);

    flag_count = strlen(argv[1]);
    for (i = 1; i < flag_count; i++) {
        switch (argv[1][i]) {
            case 'a':
                attr |= FS_APPEND_FL;
                break;
            case 'c':
                attr |= FS_COMPR_FL;
                break;
            case 'D':
                attr |= FS_DIRSYNC_FL;
                break;
            case 'i':
                attr |= FS_IMMUTABLE_FL;
                break;
            case 'j':
                attr |= FS_JOURNAL_DATA_FL;
                break;
            case 'A':
                attr |= FS_NOATIME_FL;
                break;
            case 'd':
                attr |= FS_NODUMP_FL;
                break;
            case 't':
                attr |= FS_NOTAIL_FL;
                break;
            case 's':
                attr |= FS_SECRM_FL;
                break;
            case 'S':
                attr |= FS_SYNC_FL;
                break;
            case 'T':
                attr |= FS_TOPDIR_FL;
                break;
            case 'u':
                attr |= FS_UNRM_FL;
                break;
            default:
                fprintf(stderr, "ERROR: unknown flag %c\n", argv[1][i]);
                exit(EXIT_FAILURE);
        }
    }
    do_ioctl(fd, FS_IOC_SETFLAGS, &attr);
    
    exit(EXIT_SUCCESS);
}

