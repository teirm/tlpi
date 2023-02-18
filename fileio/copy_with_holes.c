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
main(int argc, char *argv[])
{

    int input_fd;
    int output_fd;
    int open_flags;
    int num_read;
    int hole_counter;
    int write_counter;
    int rc = 0;
    mode_t mode_flags;
    char *bufp = NULL;
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
        while (num_read > 0) {
            while (*bufp != '\0' && write_counter < num_read) {
                write_counter++;
            }
            if (write(output_fd, bufp, write_counter) != write_counter) {
                fatal("could not write buffer segment");
            }
            bufp += write_counter;
            num_read -= write_counter;
            write_counter = 0;

            while (*bufp == '\0' && hole_counter < num_read) {
                bufp++;
                hole_counter++;
            }
            if (hole_counter != 0) {
                rc = lseek(output_fd, hole_counter, SEEK_CUR);
                if (rc == -1) {
                    errExit("lseek");
                }
            }
            num_read -= hole_counter;
            hole_counter = 0;
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
