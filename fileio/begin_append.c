/*
 * Exercise 5-2 from LPI
 *
 * 09 November 2018
 */

#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>

#include <lib/tlpi_hdr.h>

int 
main(int argc, char *argv[])
{
    int fd; 
    
    if (argc != 2 || strcmp(argv[1], "--help") == 0) {
        usageErr("%s <file>\n", argv[0]);
    }
    
    fd = open(argv[1], O_WRONLY | O_APPEND);
    if (fd == -1) {
        errExit("failed to open file %s", argv[1]);
    }
    
    if (lseek(fd, SEEK_SET, 0) == -1) {
        errExit("lseek");
    }
    
    if(write(fd, "test", 4) == -1) {
        errExit("write");
    }

    exit(EXIT_SUCCESS);
}
