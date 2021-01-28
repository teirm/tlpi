/*
 * Example program using select to monitor multiple file
 * descriptors.
 *
 * 1 June 2020
 */

#include <sys/time.h>
#include <sys/select.h>

#include <lib/tlpi_hdr.h>

static void
usageError(const char *prog_name)
{
    fprintf(stderr, "Usage: %s {timeout|-} fd-num[rw]...\n", prog_name);
    fprintf(stderr, "    - means infinite timeout; \n");
    fprintf(stderr, "    r = monitor for read\n");
    fprintf(stderr, "    w = monitor for write\n\n");
    fprintf(stderr, "    e.g.: %s - 0rw 1w\n", prog_name);
    exit(EXIT_FAILURE);
}

int
main(int argc, char *argv[])
{
    fd_set read_fds;
    fd_set write_fds;
    int ready;
    int n_fds;
    int fd;
    int num_read;
    int j;
    struct timeval timeout;
    struct timeval *poll_timeout = NULL;

    char buf[10]; // large nough to hold "rw\0" 

    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        usageError(argv[0]);
    }
    
    /* timeout for select() is specified in argv[1] */

    if (strcmp(argv[1], "-") == 0) {
        poll_timeout = NULL;
    } else {
        poll_timeout = &timeout;
        timeout.tv_sec = getLong(argv[1], 0, "timeout");
        timeout.tv_usec = 0;
    }

    /* process remaining arguments to build file descriptor sets */
    n_fds = 0;
    FD_ZERO(&read_fds);
    FD_ZERO(&write_fds);

    for (j = 2; j < argc; j++) {
        num_read = sscanf(argv[j], "%d%2[rw]", &fd, buf);
        if (num_read != 2) {
            usageError(argv[0]);
        }
        if (fd >= FD_SETSIZE) {
            cmdLineErr("file descriptor exceeds limit (%d)\n", FD_SETSIZE);
        }
        
        if (fd >= n_fds) {
            n_fds = fd + 1;
        }
        if (strchr(buf, 'r') != NULL) {
            FD_SET(fd, &read_fds);
        }
        if (strchr(buf, 'w') != NULL) {
            FD_SET(fd, &write_fds);
        }
    }
    
    /* call select() */
    ready = select(n_fds, &read_fds, &write_fds, NULL, poll_timeout);
    if (ready == -1) {
        errExit("select");
    }

    /* display results */
    for (fd = 0; fd < n_fds; fd++) {
        printf("%d: %s%s\n", fd, FD_ISSET(fd, &read_fds) ? "r" : "",
                FD_ISSET(fd, &write_fds) ? "w" : "");
    }
    
    if (poll_timeout != NULL) {
        printf("timeout after select(): %ld.%03ld\n",
                (long) timeout.tv_sec, (long) timeout.tv_usec / 1000);
    }
    exit(EXIT_SUCCESS);
}
