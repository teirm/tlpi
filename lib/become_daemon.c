/*
 * become_daemon.c
 *
 * creating a daemon process.
 */

#include <sys/stat.h>
#include <fcntl.h>
#include <daemons/become_daemon.h>
#include <lib/tlpi_hdr.h>

int becomeDaemon(int flags)
{
    int max_fd;
    int fd;

    switch (fork()) {
    case -1:
        return -1;
    case 0:
        break;
    default: _exit(EXIT_SUCCESS);
    }

    if (setsid() == -1)
        return -1;

    switch (fork()) {
    case -1: return -1;
    case 0: break;
    default: _exit(EXIT_SUCCESS);
    }

    if (!(flags & BD_NO_UMASK0))
        umask(0);

    if (!(flags & BD_NO_CHDIR))
        chdir("/");
    
    if (!(flags & BD_NO_CLOSE_FILES)) {
        max_fd = sysconf(_SC_OPEN_MAX);
        if (max_fd == -1)
            max_fd = BD_MAX_CLOSE;

        for (fd = 0; fd < max_fd; fd++)
            close(fd);
    }

    if (!(flags & BD_NO_REOPEN_STD_FDS)) {
        close(STDIN_FILENO);

        fd = open("/dev/null", O_RDWR);

        if (fd != STDIN_FILENO)
            return -1;
        if (dup2(STDIN_FILENO, STDOUT_FILENO) != STDOUT_FILENO)
            return -1;
        if (dup2(STDIN_FILENO, STDERR_FILENO) != STDERR_FILENO)
            return -1;
    }

    return 0;
}
