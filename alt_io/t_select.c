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
    struct timeval *pto = NULL;

    char buf[10]; // large nough to hold "rw\0" 

    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        usageError(argv[0]);
    }
}
