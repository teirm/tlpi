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
