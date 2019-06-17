/*
 * atomic_append.c 
 *
 * Exercise 5.3 lpi
 */

#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

#include <sys/stat.h>
#include <sys/types.h>

#include <lib/tlpi_hdr.h>

int
main(int argc, char *argv[])
{
    int fd;
    int open_flags;
    mode_t mode_flags;
    int write_count = 0;
    int lseek_flag = FALSE;

    if ((argc != 3 && argc != 4) || strcmp(argv[1], "--help") == 0) {
        usageErr("%s filename num-bytes [x]\n", argv[0]);
    }
    
    if (argc == 4 && strcmp(argv[3], "x") == 0) {
        lseek_flag = TRUE;
    }

    if (lseek_flag) {
        open_flags = O_WRONLY | O_CREAT; 
    } else {
        open_flags = O_WRONLY | O_APPEND | O_CREAT;
    }

    mode_flags = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |
                 S_IROTH | S_IWOTH;

    fd = open(argv[1], open_flags, mode_flags);
    if (fd == -1) {
        errExit("Failed to open %s\n", argv[1]);
    }

    write_count = atoll(argv[2]);

    
    for (int i = 0; i < write_count; i++) {
        if (lseek_flag) {
            if(lseek(fd, 0, SEEK_END) == -1) {
                errExit("lseek");
            }
        }
        
        if(write(fd, "c", 1) == -1) {
            errExit("write");
        }
    }
    
    exit(EXIT_SUCCESS);
}
