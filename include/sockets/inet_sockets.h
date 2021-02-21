#ifndef INET_SOCKETS_H
#define INET_SOCKETS_H

#include <sys/socket.h>
#include <netdb.h>

int inet_connect(const char *host, const char *service, int type);

int inet_listen(const char *service, int backlog, socklen_t *addrlen);

int inet_bind(const char *service, int type, socklen_t *addrlen);

char *inet_address_str(const struct sockaddr *addr, socklen_t addr_len, char *addr_str, int addr_str_len);


/* 
 * suggested length for string buffer that caller should pass to 
 * inet_address_str(). Must be greater than (NI_MAXHOST + NI_MAXSERV + 4)
 */
#define IS_ADDR_STR_LEN 4096

#endif /* INET_SOCKETS_H */
