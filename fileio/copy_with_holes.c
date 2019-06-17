/*
 * A program to copy a file while also 
 * keeping write holes.
 *
 * 07 November 2018
 */


#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <lib/tlpi_hdr.h>

#ifndef BUF_SIZE
#define BUF_SIZE 1024
#endif

int
buf_is_hole(char *buf, int num_read)
{
    int i;
    int is_hole = 1;
    
    for (i = 0; i < num_read; ++i) {
        if (buf[i] != '\0') {
            is_hole = 0;
            break;
        }
    }
    
    return is_hole;
}

int
main(int argc, char *argv[])
{
    
    int input_fd;
    int output_fd;
    int open_flags; 
    int null_counter = 0;
    int num_read;
    int rc = 0;
    mode_t mode_flags;

    char buf[BUF_SIZE];

    if (argc != 3 || strcmp(argv[1], "--help") == 0) {
        usageErr("%s old-file new-file\n", argv[0]);
    }
    
    input_fd = open(argv[1], O_RDONLY);
    if (input_fd == -1) {
        errExit("opening file %s", argv[1]);
    }
    
    open_flags = O_CREAT | O_WRONLY | O_TRUNC;
    mode_flags = S_IWUSR | S_IRUSR | S_IWGRP | S_IRGRP |
                 S_IWOTH | S_IROTH;

    output_fd = open(argv[2], open_flags, mode_flags);
    if (output_fd == -1) {
        errExit("opening file %s", argv[2]);
    }

    while ((num_read = read(input_fd, buf, BUF_SIZE)) > 0) {
        if (buf_is_hole(buf, num_read)) {
            rc = lseek(output_fd, num_read, SEEK_CUR);
            if (rc == -1) {
                errExit("lseek");
            }
        } else {
            if (write(output_fd, buf, num_read) != num_read) {
                fatal("could not write whole buffer");
            }
        }
    }

    if (num_read == -1) {
        errExit("read");
    }
    
    if (close(input_fd) == -1) {
        errExit("close");
    }

    if (close(output_fd) == -1) {
        errExit("close");
    }
    
    exit(EXIT_SUCCESS);
}
