/* id echo sv */

#include <syslog.h>
#include <daemons/become_daemon.h>

#include "id_echo.h"

int
main(int argc, char *argv[])
{
    int socket_fd = 0;
    ssize_t num_read = 0;
    socklen_t len = 0;
    struct sockaddr_storage client_addr;
    char buf[BUF_SIZE];
    char addr_str[IS_ADDR_STR_LEN];

    memset(buf, 0, sizeof(buf));
    memset(addr_str, 0, sizeof(addr_str));

    if (becomeDaemon(0) == -1) {
        errExit("becomeDaemon");
    }

    socket_fd = inet_bind(SERVICE, SOCK_DGRAM, NULL);
    if (socket_fd == -1) {
        syslog(LOG_ERR, "Could not create server socket (%s)", strerror(errno));
        exit(EXIT_FAILURE);
    }

    syslog(LOG_DEBUG, "Socket fd: %d\n", socket_fd);

    /* Receive datagrams and return copies to senders */
    for (;;) {
        len = sizeof(struct sockaddr_storage);
        num_read = recvfrom(socket_fd, 
                            buf, 
                            BUF_SIZE, 
                            0, 
                            (struct sockaddr *)&client_addr, 
                            &len);
        if (num_read == -1) {
            errExit("recvfrom");
        }

        if (sendto(socket_fd, buf, num_read, 0, (struct sockaddr *)&client_addr, len) != num_read) {
            syslog(LOG_WARNING, 
                   "Error echoing response to %s (%s)", 
                   inet_address_str((struct sockaddr *)&client_addr, len, addr_str, IS_ADDR_STR_LEN), strerror(errno));
        }
    }
}
