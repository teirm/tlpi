/*
 * Exercise 5.7 from LPI.
 *
 * Own implementation of scatter-gather io 
 * in the form of readv and writev.
 *
 * 13 November 2018
 */

#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/uio.h>


#include <lib/tlpi_hdr.h>

size_t
my_readv(int fd, const struct iovec *iov, int iovcnt)
{
    int bytes_read = 0;
    int remaining_buffer = 0;
    int buffer_offset = 0;
    size_t total_read = 0;
    for (int i = 0; i < iovcnt; i++) {
        buffer_offset = 0;
        remaining_buffer = iov[i].iov_len;
        while (remaining_buffer) { 
            bytes_read = read(fd, iov[i].iov_base + buffer_offset, remaining_buffer); 
           
            /* Return in case of error or EOF */
            if (bytes_read == -1 || bytes_read == 0) {
                perror("Error");
                return bytes_read; 
            }

            remaining_buffer -= bytes_read;
            buffer_offset += bytes_read;
            total_read += bytes_read;
        }
    }
    return total_read;
}


int 
my_writev(int fd, const struct iovec *iov, int iovcnt)
{
    int i;
    int total_bytes = 0;
    int bytes_written = 0;
    size_t buffer_offset = 0;
    char *unified_write_buffer = NULL; 

    /* compute the total number of bytes 
     * in all the buffers */
    for (i = 0; i < iovcnt; i++) {
        total_bytes += iov[i].iov_len;
    }

    /* Allocate a buffer to copy all buffers into */
    unified_write_buffer = (char *)malloc(total_bytes * sizeof(char));
    
    if (unified_write_buffer == NULL) {
        /* malloc failed return -1 */
        return -1;
    }
    
    /* copy all the buffers */
    for (i = 0; i < iovcnt; i++) {
        memcpy(unified_write_buffer + buffer_offset, iov[i].iov_base, iov[i].iov_len);  
        buffer_offset += iov[i].iov_len; 
    }
    
    /* write unified_write_buffer */
    return  write(fd, unified_write_buffer, total_bytes);
}

/* Test program for the above two calls */

int 
main(int argc, char *argv[])
{
    int input_fd;
    int output_fd;
    int open_flags;
    mode_t mode_flags;

    char buf_a[5];
    char buf_b[6];
    char buf_c[4]; 

    struct iovec iov[3];
    size_t bytes_read = 0;
    size_t bytes_written = 0;

    if (argc != 3) {
        usageErr("%s <in_file> <out_file>", argv[0]);
    }
    
    input_fd = open(argv[1], O_RDONLY);
    if (input_fd == -1) {
        errExit("failed to open file %s", argv[1]);
    }
   
    open_flags = O_CREAT | O_WRONLY;
    mode_flags = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |
                 S_IROTH | S_IWOTH;

    output_fd = open(argv[2], open_flags, mode_flags);

    iov[0].iov_base = buf_a;
    iov[0].iov_len = 5;

    iov[1].iov_base = buf_b;
    iov[1].iov_len = 6;

    iov[2].iov_base = buf_c;
    iov[2].iov_len = 4;

    bytes_read = my_readv(input_fd, iov, 3);
    printf("Bytes read: %ld\n", (long)bytes_read);
    
    bytes_written = my_writev(output_fd, iov, 3);
    printf("Bytes written: %ld\n", (long)bytes_written);

    return 0;
}
