/*
 * pipe_benchmark.c
 *
 * Benchmarking pipes for different block sizes
 *
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <errno.h>

#include <lib/tlpi_hdr.h>


#define MAX_ITERATIONS 100

static int display_bandwidth(long transfer_size, struct timespec *start, struct timespec *end)
{
    long elapsed_time;
    double bandwidth;

    if (start == NULL) {
        return -1;
    }

    if (end == NULL) {
        return -1;
    }
    
    elapsed_time = end->tv_nsec - start->tv_nsec;
    bandwidth = (transfer_size * 1e9) / elapsed_time; 

    printf("bandwidth (b/s); %lf\n", bandwidth);

    return 0;
}

static char *get_buffer(long buffer_size) {

    return malloc(buffer_size * sizeof(char));
}

static void child_writer(int *file_des, long buffer_size)
{
    int i; 
    char *buffer; 
    ssize_t bytes_written;

    /* close read pipe */
    if (close(file_des[0]) == -1) {
        fatal("CHILD -- failed to close read pipe\n"); 
    }
       
    buffer = get_buffer(buffer_size);
    if (buffer == NULL) {
        fatal("CHILD -- failed to allocate buffer\n");
    }

    for (i = 0; i < MAX_ITERATIONS; i++) {
        bytes_written = write(file_des[1], buffer, buffer_size);
        if (bytes_written != buffer_size) {
            fatal("CHILD -- failed to write full buffer: %ld\n", 
                    (long)bytes_written);
        }
    }      
   
    free(buffer);

    exit(EXIT_SUCCESS);
}

static void parent_reader(int *file_des, long buffer_size)
{
    int i;
    int rc;
    char *buffer;
    ssize_t bytes_read;
    
    struct timespec start_time;
    struct timespec end_time;

    if (close(file_des[1]) == -1) {
        fatal("PARENT -- failed to close write pipe\n");
    }

    buffer = get_buffer(buffer_size);
    if (buffer == NULL) {
        fatal("PARENT -- failed to allocate buffer\n");
    }
    
    rc = clock_gettime(CLOCK_MONOTONIC, &start_time);
    if (rc == -1) {
        fatal("PARENT -- failed to get start time: %s\n", strerror(errno));
    }

    for (i = 0; i < MAX_ITERATIONS; i++) {
        bytes_read = read(file_des[0], buffer, buffer_size);
        if (bytes_read != buffer_size) {
            fatal("PARENT -- failed to read full buffer: %ld\n",
                    (long)bytes_read);
        }
    }

    rc = clock_gettime(CLOCK_MONOTONIC, &end_time);
    if (rc == -1) {
        fatal("PARENT -- failed to get end time: %s\n", strerror(errno));
    }
    
    rc = display_bandwidth(MAX_ITERATIONS * buffer_size, &start_time, &end_time);
    if (rc == -1) {
        fatal("PARENT -- failed to compute bandwidth\n");
    }

    free(buffer);

    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
    int rc;
    int file_des[2];
    long buffer_size;
    char *buffer;
    struct timespec start_time;
    struct timespec end_time;
    long elapsed_ns;

    if (argc != 2) {
        usageErr("%s <buffer-size>\n", argv[0]);
    }
    
    rc = pipe(file_des);
    if (rc == -1) {
        errExit("Failed to create pipe\n");
    }

    buffer_size = getLong(argv[1], GN_NONNEG | GN_GT_0, "block_size");

    switch (fork()) {
        case -1:
            errExit("fork()\n");
        case 0:
            child_writer(file_des, buffer_size);
            break;
        default:
            parent_reader(file_des, buffer_size);
            break;
    }
    
}
