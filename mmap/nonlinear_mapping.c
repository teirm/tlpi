/*
 * Non-linear mapping without 
 * using remap_file_pages.
 *
 * 8 July 2019
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>

#include <lib/tlpi_hdr.h>

#define PAGE_SIZE 4096

int
main(int argc, char *argv[])
{
    int test_fd;
    
    char quit;
    char *anon_mapping;
    char *test_mapping;

    size_t section_size;

    struct stat test_sb;
    
    if (argc != 2 || strcmp(argv[1], "--help") == 0) {
        usageErr("./nonlinear_mapping <file>\n");
    }

    test_fd = open(argv[1], O_RDWR);
    if (test_fd == -1) {
        errExit("open");
    }

    if (fstat(test_fd, &test_sb) == -1) {
        errExit("fstat");
    }

    anon_mapping = mmap(NULL,
                        test_sb.st_size, 
                        PROT_WRITE,
                        MAP_PRIVATE | MAP_ANONYMOUS,
                        -1,
                        0);
    if (anon_mapping == MAP_FAILED) {
        errExit("mmap1");
    }
   
    test_mapping = mmap(anon_mapping,
                        PAGE_SIZE,
                        PROT_READ,
                        MAP_PRIVATE | MAP_FIXED, 
                        test_fd,
                        PAGE_SIZE << 1);

    if (test_mapping == MAP_FAILED) {
        errExit("mmap2");
    }

    test_mapping = mmap(anon_mapping + PAGE_SIZE,
                        PAGE_SIZE,
                        PROT_WRITE,
                        MAP_PRIVATE | MAP_FIXED,
                        test_fd,
                        PAGE_SIZE);
    if (test_mapping == MAP_FAILED) {
        errExit("mmap3");
    }

    test_mapping = mmap(anon_mapping + (PAGE_SIZE << 1),
                        PAGE_SIZE,
                        PROT_WRITE,
                        MAP_PRIVATE | MAP_FIXED,
                        test_fd,
                        0);
    if (test_mapping == MAP_FAILED) {
        errExit("mmap4");
    }
  
    if (close(test_fd) == -1) {
        errExit("close");
    }
   
    printf("Check /proc/%d/maps\nPress any key after to exit.\n", getpid());
    scanf("%c", &quit);

    munmap(test_mapping + (PAGE_SIZE << 1), PAGE_SIZE);
    munmap(test_mapping + PAGE_SIZE, PAGE_SIZE);
    munmap(test_mapping, PAGE_SIZE);

    munmap(anon_mapping, test_sb.st_size);

    exit(EXIT_SUCCESS);
}
