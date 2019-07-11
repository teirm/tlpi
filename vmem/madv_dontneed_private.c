/*
 * madv_dont_need_private.c
 *
 * Demonstrating the reinit behavior
 * of MADV_DONTNEED on a MAP_PRIVATE
 * mapping
 *
 * 10 July 2019
 */

#include <sys/mman.h>
#include <lib/tlpi_hdr.h>

#define MAP_SIZE 4096

int 
main(int argc, char *argv[])
{
    char *mapping;
    long page_size;

    page_size = sysconf(_SC_PAGESIZE);
    if (page_size == -1) {
        errExit("sysconf(_SC_PAGESIZE)");
    }

    mapping = mmap(NULL, page_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (mapping == MAP_FAILED) {
        errExit("mmap");
    }

    /* scribble something into the mapping */
    mapping[0] = 'd';
    mapping[1] = 'o';
    mapping[2] = 'g';

    /* verify values */
    printf("%c%c%c\n", mapping[0], mapping[1], mapping[2]);

    if (madvise(mapping, page_size, MADV_DONTNEED) == -1) {
        errExit("madvise");
    }

    /* check values again */
    printf("%c%c%c\n", mapping[0], mapping[1], mapping[2]);
    
    return 0;
}

