/* inet_sockets library taken from TLPI */

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <sockets/inet_sockets.h>
#include <lib/tlpi_hdr.h>

#define memzero(_addr_, _size_) memset(_addr_, 0, _size_)

/**
 * private interface for inet_bind() and inet_listen()
 */
static int inet_passive_socket(const char *service, int type, socklen_t *addrlen, Boolean do_listen, int backlog)
{
    struct addrinfo hints;
    struct addrinfo *result = NULL;
    struct addrinfo *rp = NULL;
    int socket_fd = 0;
    int optval = 0;
    int rc = 0;
    
    memzero(&hints, sizeof(hints));
    
    hints.ai_canonname = NULL;
    hints.ai_addr      = NULL;
    hints.ai_next      = NULL;
    hints.ai_socktype  = type;
    hints.ai_family    = AF_UNSPEC;  /* allow IPv4 or IPv6 */
    hints.ai_flags     = AI_PASSIVE; /* Use wildcard IP address */

    rc = getaddrinfo(NULL, service, &hints, &result);
    if (rc != 0) {
        errno = ENOSYS;
        return -1;
    }

    optval = 1;
    /* 
     * iterate over list still an address structure is found
     * that can be connected to and bound
     */
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        socket_fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (socket_fd == -1) {
            continue;
        }
        
        if (do_listen) {
            if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
                close(socket_fd);
                freeaddrinfo(result);
                return -1;
            }
        }
        
        if (bind(socket_fd, rp->ai_addr, rp->ai_addrlen) == 0) {
            break;
        }

        close(socket_fd);
    }
    
    if (rp != NULL && do_listen) {
        if (listen(socket_fd, backlog) == -1) {
            freeaddrinfo(result);
            return -1;
        }
    }

    if (rp != NULL && addrlen != NULL) {
        *addrlen = rp->ai_addrlen;
    }

    freeaddrinfo(result);
    return (rp == NULL) ? -1 : socket_fd;
}

/**
 * creates a socket with given type and connects it to
 * address at host and service
 */
int inet_connect(const char *host, const char *service, int type)
{
    struct addrinfo hints;
    struct addrinfo *result   = NULL;
    struct addrinfo *rp       = NULL;
    int             socket_fd = 0;
    int             rc        = 0;

    memzero(&hints, sizeof(hints));
    
    hints.ai_canonname = NULL;
    hints.ai_addr      = NULL;
    hints.ai_next      = NULL;
    hints.ai_family    = AF_UNSPEC; /* allow IPv4 or IPv6 */
    hints.ai_socktype  = type;

    rc = getaddrinfo(host, service, &hints, &result);
    if (rc != 0) {
        errno = ENOSYS;
        return -1;
    }

    /* 
     * iterate over list still an address structure is found
     * that can be connected to
     */
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        socket_fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (socket_fd == -1) {
            continue;
        }

        if (connect(socket_fd, rp->ai_addr, rp->ai_addrlen) != -1) {
            break;
        }

        close(socket_fd);
    }

    freeaddrinfo(result);

    return (rp == NULL) ? -1 : socket_fd;
}

int inet_listen(const char *service, int backlog, socklen_t *addrlen)
{
    return inet_passive_socket(service, SOCK_STREAM, addrlen, TRUE, backlog);
}

int inet_bind(const char *service, int type, socklen_t *addrlen)
{
    return inet_passive_socket(service, type, addrlen, FALSE, 0);
}

char *inet_address_str(const struct sockaddr *addr, socklen_t addr_len, char *addr_str, int addr_str_len)
{
    char host[NI_MAXHOST];
    char service[NI_MAXSERV];

    memzero(host, sizeof(host));
    memzero(service, sizeof(service));

    if (getnameinfo(addr, addr_len, host, NI_MAXHOST, service, NI_MAXSERV, NI_NUMERICSERV) == 0) {
        snprintf(addr_str, addr_str_len, "(%s %s)", host, service);
    } else {
        snprintf(addr_str, addr_str_len, "(?UNKNOWN?)");
    }

    return addr_str;
}
