/*
 * failed_chmod.c 
 *
 * Problem 18-2 in tlpi
 */

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>

#include <sys/stat.h>

int main()
{
    int fd;

    mkdir("test", S_IRUSR | S_IWUSR | S_IXUSR);
    chdir("test");
    fd = open("myfile", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    symlink("myfile", "../mylink");
    chmod("../mylink", S_IRUSR);
    perror("ERROR");
}
