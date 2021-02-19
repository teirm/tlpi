/* sequence number server */

/* needed to get NI_MAX_HOST and NI_MAXSERV from <netdb.h> */
#define _DEFAULT_SOURCE

#include <netdb.h>
#include "is_seqnum.h"

#define BACKLOG 50

#define memzero(_addr_, _size_) memset(_addr_, 0, _size_)

int
main(int argc, char *argv[])
{
    uint32_t seq_num = 0;
    int listen_fd = 0;
    int client_fd = 0;
    int optval = 0;
    int req_len = 0;
    char req_len_str[INT_LEN];
    char seq_num_str[INT_LEN];
    struct sockaddr_storage client_addr;
    socklen_t addr_len;
    struct addrinfo hints;
    struct addrinfo *result = NULL;
    struct addrinfo *rp = NULL;
#define ADDRSTRLEN (NI_MAXHOST + NI_MAXSERV + 10)
    char addr_str[ADDRSTRLEN];
    char host[NI_MAXHOST];
    char service[NI_MAXSERV];

    memzero(req_len_str, sizeof(req_len_str));
    memzero(seq_num_str, sizeof(seq_num_str));
    memzero(&client_addr, sizeof(client_addr));
    memzero(&hints, sizeof(hints));
    memzero(addr_str, sizeof(addr_str));
    memzero(service, sizeof(service));

    if (argc > 1 && strcmp(argv[1], "--help") == 0) {
        usageErr("%s [init-seq-num]\n", argv[0]);
    }

    seq_num = (argc > 1) ? getInt(argv[1], 0, "init-seq-num") : 0;

    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
        errExit("signal");
    }

    /* Call getaddrinf() to obtain a list of address that we can try binding to */
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;
    hints.ai_socktype = SOCK_STREAM;
    /* allow either IPv4 or IPv6 */
    hints.ai_family = AF_UNSPEC;
    hints.ai_flags = AI_PASSIVE | AI_NUMERICSERV;
    
    if (getaddrinfo(NULL, PORT_NUM, &hints, &result) != 0) {
        errExit("getaddrinfo");
    }
    
    /* 
     * iterate over list until an address structure that can be used
     * to create and bind a socket is found 
     */
    optval = 1;
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        listen_fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (listen_fd == -1) {
            continue;
        }
        if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
            errExit("setsockopt");
        }
        if (bind(listen_fd, rp->ai_addr, rp->ai_addrlen) == 0) {
            /* success */
            break;
        }
        /* bind failed -- close and try next */
        close(listen_fd);
    }

    if (rp == NULL) {
        fatal("Could not bind socket to any address");
    }

    if (listen(listen_fd, BACKLOG) == -1) {
        errExit("listen");
    }
    
    freeaddrinfo(result);
    
    /* iteratively handle clients */
    for (;;) {
        /* Accept a client connection, obtaining client's address */
        addr_len = sizeof(struct sockaddr_storage);
        client_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &addr_len);
        if (client_fd == -1) {
            errMsg("accept");
            continue;
        }

        if (getnameinfo((struct sockaddr *)&client_addr, addr_len, host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0) {
            snprintf(addr_str, ADDRSTRLEN, "(%s, %s)", host, service);
        } else {
            snprintf(addr_str, ADDRSTRLEN, "(?UNKNOWN?)");
        }
        printf("Connection from %s\n", addr_str);
        
        /* read client request and send sequence back */
        if (readLine(client_fd, req_len_str, INT_LEN) <= 0) {
            close(client_fd);
            continue;
        }

        req_len = atoi(req_len_str);
        if (req_len <= 0) {
            close(client_fd);
            continue;
        }

        snprintf(seq_num_str, INT_LEN, "%d\n", seq_num);
        if (write(client_fd, &seq_num_str, strlen(seq_num_str)) != strlen(seq_num_str)) {
            fprintf(stderr, "error on write to %s", addr_str);
        }
        
        seq_num += req_len;

        if (close(client_fd) == -1) {
            errMsg("close");
        }
    }
}
