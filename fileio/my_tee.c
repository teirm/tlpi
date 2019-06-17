/*
 * my_tee.c
 *
 * Linux programming interface exercise 
 * 4-1
 *
 * 06 Nov 2018
 *
 */

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <lib/tlpi_hdr.h>

/* allow "cc -D" to override definition */
#ifndef BUF_SIZE
#define BUF_SIZE    1024
#endif

int 
main(int argc, char *argv[])
{
    int opt;
    int output_fd;
    int append_flag = FALSE;
    int file_index = 1;
    int open_flags;
    mode_t file_perms;
    ssize_t num_read;

    char buf[BUF_SIZE];

    if (argc < 2 || argc > 3 || strcmp(argv[1], "--help") == 0) {
        usageErr("%s [-a] new-file\n", argv[0]);
    }

    while ((opt = getopt(argc, argv, "a")) != -1) {
        switch(opt) {
            case 'a':
                append_flag = TRUE;
                break;
            default:
                usageErr("%s [-a] new-file\n", argv[0]);
                break;
        }
    }
    
    if (append_flag) {
        file_index = 2;
        open_flags = O_CREAT | O_WRONLY | O_APPEND;
    } else {
        open_flags = O_CREAT | O_WRONLY | O_TRUNC;
    }

    file_perms = S_IRUSR | S_IWUSR | S_IRGRP |
                 S_IWGRP | S_IROTH | S_IWOTH;

    output_fd = open(argv[file_index], open_flags, file_perms);
    if (output_fd == -1) {
        errExit("opening file %s", argv[1]);
    }

    /* transfer data till end of input or an error */

    while ((num_read = read(STDIN_FILENO, buf, BUF_SIZE)) > 0) {
        if (write(output_fd, buf, num_read) != num_read) {
            fatal("could not write whole buffer");
        }
    }

    if (num_read == -1) {
        errExit("read");
    }

    if (close(output_fd) == -1) {
        errExit("close output");
    }

    exit(EXIT_SUCCESS);
}
