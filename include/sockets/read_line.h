/* 
 * reading data one line at a time
 * taken from TLPI
 */

#include <unistd.h>

ssize_t readLine(int fd, void *buffer, size_t n);
