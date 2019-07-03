/*
 * mmcat.c
 *
 * cat(1) implemented using mmap
 */

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <lib/tlpi_hdr.h>

int
main(int argc, char *argv[])
{
    char *addr;
    int fd;
    struct stat sb;

    if (argc != 2 || strcmp(argv[1], "--help") == 0) {
        usageErr("%s file\n", argv[0]);
    }

    fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        errExit("open");
    }

    /* obtain the size of the file and use it to specify
     * size of mapping and the size of the buffer to be 
     * written.
     */

    if (fstat(fd, &sb) == -1) {
        errExit("fstat");
    }

    addr = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (addr == MAP_FAILED) {
        errExit("mmap");
    }

    if (write(STDOUT_FILENO, addr, sb.st_size) != sb.st_size) {
        fatal("partial/failed write");
    }
    exit(EXIT_SUCCESS);
}
