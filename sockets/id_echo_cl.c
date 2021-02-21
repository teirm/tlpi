/* echo client */

#include "id_echo.h"

int main(int argc, char *argv[])
{
    int socket_fd = 0;
    int j = 0;
    size_t len = 0;
    ssize_t num_read = 0;
    char buf[BUF_SIZE];

    memset(buf, 0, sizeof(buf));

    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        usageErr("%s host msg...\n", argv[0]);
    }

    /* construct server address from first command line argument */
    socket_fd = inet_connect(argv[1], SERVICE, SOCK_DGRAM);
    if (socket_fd == -1) {
        fatal("could not connect to server socket");
    }

    /* send remaining args to server as separate datagrams */
    for (j = 2; j < argc; j++) {
        len = strlen(argv[j]);
        if (write(socket_fd, argv[j], len) != len) {
            fatal("partial/failed write");
        }

        num_read = read(socket_fd, buf, BUF_SIZE);
        if (num_read == -1) {
            errExit("read");
        }

        printf("[%ld bytes] %.*s\n", (long)num_read, (int)num_read, buf);
    }
    
    exit(EXIT_SUCCESS);
}
