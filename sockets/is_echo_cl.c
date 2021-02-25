/* client for echo service */

#include <sockets/inet_sockets.h>
#include <lib/tlpi_hdr.h>

#define BUF_SIZE    100

int main(int argc, char *argv[])
{
    int     socket_fd = 0;
    ssize_t num_read  = 0;
    char    buf[BUF_SIZE];

    memset(buf, 0, sizeof(buf));

    if (argc != 2 || strcmp(argv[1], "--help") == 0) {
        usageErr("%s host\n", argv[0]);
    }

    socket_fd = inet_connect(argv[1], "echo", SOCK_STREAM);
    if (socket_fd == -1) {
        errExit("inet_connect");
    }

    switch (fork()) {
    case -1:
        /* error */
        errExit("fork");

    case 0:
        /* child: read server's response and echo to stdout */
        for (;;) {
            num_read = read(socket_fd, buf, BUF_SIZE);
            if (num_read <= 0) {
                /* exit on EOF */
                break;
            }
            printf("%.*s", (int)num_read, buf);
        }
        exit(EXIT_SUCCESS);
    
    default:
        /* parent: write contents of stdin to socket */
        for (;;) {
            num_read = read(STDIN_FILENO, buf, BUF_SIZE);
            if (num_read <= 0) {
                /* exit loop on EOF or error */
                break;
            }
            if (write(socket_fd, buf, num_read) != num_read) {
                fatal("write() failed");
            }
        }

        /* close writing channel, so server sees EOF */
        if (shutdown(socket_fd, SHUT_WR) == -1) {
            errExit("shutdown");
        }
        exit(EXIT_SUCCESS);
    }
}

