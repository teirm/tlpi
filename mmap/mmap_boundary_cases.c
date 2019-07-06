/*
 * mmap_boundary_cases.c 
 *
 * Checking boundary conditions for SIGBUS 
 * and SIGSEGV with mmaps.
 *
 * 06 July 2019
 */

#include <unistd.h>
#include <fcntl.h>

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <lib/tlpi_hdr.h>

#define EXTRA_SIZE 100

int 
main(int argc, char *argv[])
{
    int test_fd;
    
    char *test_map;

    struct stat test_stat_buf;

    if (argc != 3 || strcmp(argv[1], "--help") == 0) {
        usageErr("./%s <test_file> <err>", argv[0]);
    }

    test_fd = open(argv[1], O_CREAT | O_RDWR,
                    S_IRWXU | S_IRGRP | S_IROTH);
    if (test_fd == -1) {
        errExit("open");
    }

    if (fstat(test_fd , &test_stat_buf) == -1) {
        errExit("fstat");
    }

    /*  0                   4095            5095       8191
        | file (4096 bytes) |    no file    |          |
        <------------request---------------->
        <--------------------map----------------------->
    */

    test_map = mmap(NULL, test_stat_buf.st_size + 1000,
                    MAP_PRIVATE, PROT_READ|PROT_WRITE, test_fd, 0);

    if (strcmp(argv[2], "bus") == 0) {
        *(test_map+4096) = 10;
    } else if (strcmp(argv[2], "segv") == 0) {
        *(test_map + EXTRA_SIZE + 100000) = 10;
    } else {
        munmap(test_map, test_stat_buf.st_size);
        errExit("Unknown condition"); 
    }

    return 0;
}
