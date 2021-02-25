/* use of getsockname() and getpeername() from TLPI */

#include <sockets/inet_sockets.h>
#include <lib/tlpi_hdr.h>

int main(int argc, char *argv[]) 
{
    int listen_fd = 0;
    int accept_fd = 0;
    int conn_fd   = 0;
    socklen_t len = 0;
    void     *addr = NULL;
    char addr_string[IS_ADDR_STR_LEN];

    memset(addr_string, 0, sizeof(addr_string));

    if (argc != 2 || strcmp(argv[1], "--help") == 0) {
        usageErr("%s service\n", argv[0]);
    }

    listen_fd = inet_listen(argv[1], 5, &len);
    if (listen_fd == -1) {
        errExit("inet_listen");
    }

    conn_fd = inet_connect(NULL, argv[1], SOCK_STREAM);
    if (conn_fd == -1) {
        errExit("inet_connect");
    }

    accept_fd = accept(listen_fd, NULL, NULL);
    if (accept_fd == -1) {
        errExit("accept");
    }

    addr = malloc(len);
    if (addr == NULL) {
        errExit("malloc");
    }

    if (getsockname(conn_fd, addr, &len) == -1) {
        errExit("getsockname");
    }
    printf("getsockname(conn_fd): %s\n",
            inet_address_str(addr, len, addr_string, IS_ADDR_STR_LEN));
    if (getsockname(accept_fd, addr, &len) == -1) {
        errExit("getsockname");
    }
    printf("getsockname(accept_fd): %s\n",
            inet_address_str(addr, len, addr_string, IS_ADDR_STR_LEN));
    
    if (getpeername(conn_fd, addr, &len) == -1) {
        errExit("getpeername");
    }
    printf("getpeername(conn_fd): %s\n",
            inet_address_str(addr, len, addr_string, IS_ADDR_STR_LEN));
    if (getpeername(accept_fd, addr, &len) == -1) {
        errExit("getpeername");
    }
    printf("getpeername(accept_fd): %s\n",
            inet_address_str(addr, len, addr_string, IS_ADDR_STR_LEN));

    sleep(30);
    free(addr);
    exit(EXIT_SUCCESS);
}
