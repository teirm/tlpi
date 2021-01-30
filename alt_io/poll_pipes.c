/* poll api used to monitor pipe files */

#include <time.h>
#include <poll.h>
#include <lib/tlpi_hdr.h>

int
main(int argc, char *argv[])
{
    int n_pipes     = 0;
    int j           = 0;
    int ready       = 0;
    int rand_pipe   = 0;
    int n_writes    = 0;
    int (*pipe_fds)[2];
    
    struct pollfd   *poll_fd;
    
    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        usageErr("%s num-pipes [num-writes]\n", argv[0]);
    }

    /* 
     * allocate the arrays to use -- sized based on number 
     * of pipes specified
     */
    
    n_pipes = getInt(argv[1], GN_GT_0, "num-pipes");
    
    pipe_fds = calloc(n_pipes, sizeof(*pipe_fds));
    if (pipe_fds == NULL) {
        errExit("calloc");
    }
    
    poll_fd = calloc(n_pipes, sizeof(*poll_fd));
    if (poll_fd == NULL) {
        errExit("calloc");
    }
    
    /* create pipes specified on command line */
    for (j = 0; j < n_pipes; j++) {
        if (pipe(pipe_fds[j]) == -1) {
            errExit("pipe %d", j);
        }
    }

    /* perform writes to pipes randomly */
    n_writes = (argc > 2) ? getInt(argv[2], GN_GT_0, "num-writes") : 1;
    srandom((int)time(NULL));
    for (j = 0; j < n_writes; j++) {
        rand_pipe = random() % n_pipes;
        printf("Writing to fd %3d (read fd: %3d)\n",
                pipe_fds[rand_pipe][1], pipe_fds[rand_pipe][0]);
        if (write(pipe_fds[rand_pipe][1], "a", 1) == -1) {
            errExit("write %d", pipe_fds[rand_pipe][1]);
        }
    }

    /* build fd list for poll. Contains read ends of all pipes */
    for(j = 0; j < n_pipes; j++) {
        poll_fd[j].fd = pipe_fds[j][0];
        poll_fd[j].events = POLLIN;
    }

    ready = poll(poll_fd, n_pipes, 0);
    if (ready == -1) {
        errExit("poll");
    }

    printf("poll() returned: %d\n", ready);

    /* check which pipes have data available for reading */
    for (j = 0; j < n_pipes; j++) {
        if (poll_fd[j].revents & POLLIN) {
            printf("Readable: %3d\n", poll_fd[j].fd);
        }
    }

    exit(EXIT_SUCCESS);
}
