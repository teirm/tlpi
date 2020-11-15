/*
 * Implementing dup and dup2 
 * using fcntl.
 *
 * 09 November 2018
 */

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include <lib/tlpi_hdr.h>

int
my_dup(int oldfd)
{
    if (fcntl(oldfd, F_GETFL) == -1) {
        errno = EBADF;
        return -1;
    }
    
    return fcntl(oldfd, F_DUPFD);
}

int
my_dup2(int oldfd, int newfd)
{
    if (fcntl(oldfd, F_GETFL) == -1) {
        errno = EBADF;
        return -1;
    }

    if (fcntl(newfd, F_GETFL) == -1) {
        close(newfd);
    }
    
    return fcntl(oldfd, F_DUPFD, newfd);
}

int
main()
{
    int fd = open("test_file.txt", O_CREAT);
    if (fd == -1) {
        errExit("failed to open file\n");
    }
    
    int new_fd = my_dup(fd);
    if (new_fd == -1) {
        errExit("failed my_dup\n");
    }
    close(new_fd);

    new_fd = my_dup2(new_fd, 1000);
    if (new_fd == -1) {
        errExit("failed my_dup2\n");
    }
    
    close(new_fd);
    close(fd);


    return 0;
}

