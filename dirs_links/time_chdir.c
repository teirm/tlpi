/*
 * time_chdir.c
 *
 * A program to time chdir using chdir
 * and fchdir.
 */

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include <linux/limits.h>

#ifndef MAX_ITERATIONS
#define MAX_ITERATIONS  1000
#endif

static void
time_fchdir()
{
    int i; 
    int curr_fd; 

    time_t start_time;
    time_t end_time;

    curr_fd = open(".", O_RDONLY);
    
    start_time = time(NULL); 
    for (i = 0; i < MAX_ITERATIONS; i++) {
        if (fchdir(curr_fd) == -1) {
            perror("fchdir");
            exit(EXIT_FAILURE);
        }
    }
    end_time = time(NULL);
    
    if (close(curr_fd) == -1) {
        perror("close");
        exit(EXIT_FAILURE);
    }

    printf("%s: %lu\n", __func__, end_time - start_time);
}


static void
time_chdir()
{
    int i;
    char path_buf[PATH_MAX];
    
    time_t start_time;
    time_t end_time;

    if (getcwd(path_buf, PATH_MAX) == NULL) {
        perror("getcwd");
        exit(EXIT_FAILURE);
    }

    start_time = time(NULL);
    for (i = 0; i < MAX_ITERATIONS; i++) {
        if (chdir(path_buf) == -1) {
            perror("chdir");
            exit(EXIT_FAILURE);
        }
    }
    end_time = time(NULL);

    printf("%s: %lu\n", __func__, end_time - start_time);
}

int
main()
{
    time_fchdir();
    time_chdir();

    exit(EXIT_SUCCESS);
}



