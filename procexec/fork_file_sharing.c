/*
 * fork_file_sharing.c
 * 
 * TLPI
 */

#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>

#include <lib/tlpi_hdr.h>

int
main(int argc, char *argv[])
{
    int fd;
    int flags;
    char template[] = "/tmp/testXXXXXX";

    setbuf(stdout, NULL);

    fd = mkstemp(template);
    if (fd == -1) {
        errExit("mkstemp");
    }

    printf("file offset before fork(): %lld\n",
            (long long)lseek(fd, 0, SEEK_CUR));

    flags = fcntl(fd, F_GETFL);
    if (flags == -1) {
        errExit("fcntl - F_GETFL");
    }
    printf("O_APPEND flag before fork() is: %s\n",
            (flags & O_APPEND) ? "on" : "off");

    switch (fork()) {
    case -1:
        errExit("fork");
    case 0:
        /* child: change file offset and status flags */
        if (lseek(fd, 1000, SEEK_SET) == -1) {
            errExit("lseek");
        }

        flags = fcntl(fd, F_GETFL);
        if (flags == -1) {
            errExit("fcntl - F_GETFL");
        }
        flags |= O_APPEND;
        if (fcntl(fd, F_SETFL, flags) == -1) {
            errExit("fcntl - F_SETFL");
        }
        _exit(EXIT_SUCCESS);
    default:
        /* parent: can see file changes made by child */
        if (wait(NULL) == -1) {
            errExit("wait");
        }
        printf("child has exited\n");

        printf("File offset in parent: %lld\n", (long long)lseek(fd, 0, SEEK_CUR));
        flags = fcntl(fd, F_GETFL);
        if (flags == -1) {
            errExit("fcntl - F_GETFL");
        }
        printf("O_APPEND flag in parent is: %s\n", 
                (flags & O_APPEND) ? "on" : "off");
        exit(EXIT_SUCCESS);
    }
}

