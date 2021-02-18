/* 
 * Implementation of readLine 
 * taken from TLPI
 */
#include <sockets/read_line.h>

#include <unistd.h>
#include <errno.h>

ssize_t
readLine(int fd, void *buffer, size_t n)
{
    ssize_t num_read   = 0;
    size_t  total_read = 0;
    char   *buf        = NULL;
    char    ch;
    
    if (n <= 0 || buffer == NULL) {
        errno = EINVAL;
        return -1;
    }
    
    /* pointer arithmetic is not allowed on 'void *' */
    buf = buffer;
    
    for (;;) {
        num_read = read(fd, &ch, 1);

        if (num_read == -1) {
            /* interupted or other error */
            if (errno == EINTR) {
                continue;
            } else {
                return -1;
            }
        } else if (num_read == 0) {
            /* EOF */
            if (total_read == 0) {
                return 0;
            } else {
                break;
            }
        } else {
            if (total_read < n - 1) {
                total_read++;
                *buf++ = ch;
            }
            if (ch == '\n') {
                break;
            }
        }
    }
    *buf = '\0';
    return total_read;
}
