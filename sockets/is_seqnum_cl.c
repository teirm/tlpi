/* A client using stream sockets */

#include <netdb.h>
#include "is_seqnum.h"

#define memzero(_addr_, _size_) memset(_addr_, 0, _size_)

int
main(int argc, char *argv[])
{
    char *req_len_str = NULL;
    char seq_num_str[INT_LEN];
    char client_fd;
    ssize_t num_read = 0;
    struct addrinfo hints;
    struct addrinfo *result = NULL;
    struct addrinfo *rp = NULL;

    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        usageErr("%s server-host [sequence-len]\n", argv[0]);
    }

    /* call getaddrinfo to obtain list of address that we can try
     * connecting to */

    memzero(seq_num_str, sizeof(seq_num_str));
    memzero(&hints, sizeof(hints));

    /* Call getaddrinf() to obtain a list of address that we can try binding to */
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;
    hints.ai_socktype = SOCK_STREAM;
    /* allow either IPv4 or IPv6 */
    hints.ai_family = AF_UNSPEC;
    hints.ai_flags = AI_NUMERICSERV;
    
    if (getaddrinfo(argv[1], PORT_NUM, &hints, &result) != 0) {
        errExit("getaddrinfo");
    }

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        client_fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (client_fd == -1) {
            continue;
        }

        if (connect(client_fd, rp->ai_addr, rp->ai_addrlen) != -1) {
            break;
        }

        close(client_fd);
    }

    if (rp == NULL) {
        fatal("could not connect to socket to any address");
    }

    freeaddrinfo(result);

    req_len_str = (argc > 2) ? argv[2] : "1";
    if (write(client_fd, req_len_str, strlen(req_len_str)) != strlen(req_len_str)) {
        fatal("Partial/failed write (req_len_str)");
    }
    if (write(client_fd, "\n", 1) != 1) {
        fatal("Partial/failed write (newline)");
    }

    /* read and display sequence number returned */
    num_read = readLine(client_fd, seq_num_str, INT_LEN);
    if (num_read == -1) {
        errExit("readLine");
    }
    if (num_read == 0) {
        fatal("Unexpected EOF from server");
    }

    printf("Sequence number: %s", seq_num_str);

    exit(EXIT_SUCCESS);
}
