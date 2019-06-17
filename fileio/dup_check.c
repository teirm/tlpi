/*
 * Checking that duplicate file descriptors
 * share same offset and flags.
 *
 * 09 November 2018
 */

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

#include <sys/types.h>

#include <lib/tlpi_hdr.h>

int
main(int argc, char *argv[])
{
    
    int fd_1;
    int fd_2;
    
    int open_flags_1;
    int open_flags_2;
     
    int offset_1;
    int offset_2;

    int open_flags = O_CREAT | O_WRONLY;
    mode_t mode_flags = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |
                        S_IROTH | S_IWOTH;

    if (argc != 2 || strcmp(argv[1], "--help") == 0) {
        usageErr("%s <filename>\n", argv[0]);
    }
    
    fd_1 = open(argv[1], open_flags, mode_flags);
    if (fd_1 == -1) {
        errExit("Unable to open %s\n", argv[1]);
    }
    
    fd_2 = dup(fd_1);
    if (fd_2 == -1) {
        errExit("Unable to dup %s\n", argv[1]);
    }
    
    open_flags_1 = fcntl(fd_1, F_GETFL);
    if (open_flags_1 == -1) {
        errExit("Unable to get open flags for fd_1\n");
    }

    open_flags_2 = fcntl(fd_2, F_GETFL);
    if (open_flags_2 == -1) {
        errExit("Unable to get open flags for fd_2\n");
    }

    if (write(fd_1, "test", 4) == -1) {
        errExit("write");
    }

    offset_1 = lseek(fd_1, SEEK_END, 0);
    if (offset_1 == -1) {
        errExit("lseek fd_1");
    }

    offset_2 = lseek(fd_2, SEEK_END, 0);
    if (offset_2 == -1) {
        errExit("lseek fd_2");
    }

    if (offset_2 != offset_1) {
        errExit("offset_2 != offset_1");
    } else {
        printf("offset_2: %lld\toffset_1: %lld\n", 
                (long long)offset_2, (long long)offset_1);
    }

    if (open_flags_2 != open_flags_1) {
        errExit("open_flags_2 != open_flags_1");
    } else {
        printf("open_flags_2: 0x%x\topen_flags_1: 0x%x\n", 
                open_flags_2, open_flags_1);
    }
    
    if (close(fd_2) == -1) {
        errExit("close fd_2");
    }

    if (close(fd_1) == -1) {
        errExit("close fd_1");
    }

    exit(EXIT_SUCCESS);
}
