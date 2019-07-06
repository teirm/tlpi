/*
 * Implementation of cp(1) using mmap
 *
 * 6 July 2017
 */

#include <unistd.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include <lib/tlpi_hdr.h>

int
main(int argc, char *argv[]) 
{
    int src_fd;
    int dest_fd;
    
    char *src_map;
    char *dest_map;

    struct stat src_statbuf;

    if (argc != 3 || strcmp(argv[1], "--help") == 0) {
        usageErr("./mmap_cp <src> <dest>");
    }
    
    src_fd = open(argv[1], O_RDONLY);
    if (src_fd == -1) {
        errExit("open");
    }
    
    if (fstat(src_fd, &src_statbuf) == -1) {
        errExit("fstat");
    }
    
    src_map = mmap(NULL, src_statbuf.st_size, PROT_READ, MAP_PRIVATE, src_fd, 0);
    if (src_map == MAP_FAILED) {
        errExit("mmap");
    }
    
    if (close(src_fd) == -1) {
        errExit("src_fd");
    }

    dest_fd = open(argv[2], O_CREAT | O_RDWR);
    if (dest_fd == -1) {
        errExit("open");
    }

    if (ftruncate(dest_fd, src_statbuf.st_size) == -1) {
        errExit("ftruncate");
    }

    dest_map = mmap(NULL, src_statbuf.st_size, PROT_WRITE, MAP_SHARED, dest_fd, 0);
    if (dest_map == MAP_FAILED) {
        errExit("mmap");
    }
    
    if (close(dest_fd) == -1) {
        errExit("close");
    }

    memcpy(dest_map, src_map, src_statbuf.st_size);

    if(msync(dest_map, src_statbuf.st_size, MS_SYNC) == -1) {
        errExit("msync");
    }

    munmap(src_map, src_statbuf.st_size);
    munmap(dest_map, src_statbuf.st_size);

    return 0;
}
    




