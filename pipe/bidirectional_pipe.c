/*
 * bidirectional_pipe.c
 *
 * A small program to demonstrate a bidirectional
 * pipe.
 */

#include <ctype.h>
#include <string.h>
#include <lib/tlpi_hdr.h> 

#define BUF_SIZE 128

static void child_function(int from_parent_fd, int to_parent_fd);
static void parent_function(int from_child_fd, int to_child_fd);

static void child_function(int from_parent_fd, int to_parent_fd)
{
    ssize_t num_read;
    char buffer[BUF_SIZE];    

    for (;;) {
        memset(buffer, 0, BUF_SIZE);

        num_read = read(from_parent_fd, buffer, BUF_SIZE);
        if (num_read == -1) {
            errExit("read - child");
        }

        if (num_read == 0) {
            errExit("read - child"); 
        }
        
        for (int i = 0; i < num_read; i++) {
            buffer[i] = toupper(buffer[i]);
        }

        if (write(to_parent_fd, buffer, num_read) != num_read) {
            fatal("child - partial/failed write");
        }
    }
    
    if (close(from_parent_fd) == -1) {
        errExit("child - close");
    }

    if (close(to_parent_fd) == -1) {
        errExit("child - close");
    }
    
    _exit(EXIT_SUCCESS);
}

static void parent_function(int from_child_fd, int to_child_fd)
{
    size_t input_len;
    ssize_t num_read;
    
    char input_buffer[BUF_SIZE];
    char read_buffer[BUF_SIZE];

    
    for (;;) {
       
        memset(input_buffer, 0, BUF_SIZE);
        memset(read_buffer, 0, BUF_SIZE);

        printf("Input: ");
        scanf("%s", input_buffer);
        input_len = strlen(input_buffer);

        if (strcmp(input_buffer, "quit") == 0) {
            break;
        }

        if (write(to_child_fd, input_buffer, input_len) != input_len) { 
            fatal("parent - partial/failed write");
        }

        num_read = read(from_child_fd, read_buffer, BUF_SIZE);
        if (num_read == -1) {
            errExit("read - parent");
        }

        if (num_read == 0) {
            errExit("read - parent");
        }
        
        printf("Out Put: %s\n", read_buffer);
    }

    if (close(to_child_fd) == -1) {
        errExit("close - parent");
    }

    if (close(from_child_fd) == -1) {
        errExit("close - parent");
    }
}

int main(int argc, char *argv[])
{
    
    int i;
    char buffer[BUF_SIZE];
    ssize_t num_read;
     
    /* 
     * parent -- write to 1 
     * child  -- read from 0
     */
    int parent_to_child_pipe_fd[2];
    
    /*
     * parent -- read from 0
     * child -- write to 1
     */
    int child_to_parent_pipe_fd[2];

    if (pipe(parent_to_child_pipe_fd) == -1) {
        errExit("pipe");
    }

    if (pipe(child_to_parent_pipe_fd) == -1) {
        errExit("pipe");
    }

    memset(buffer, 0, BUF_SIZE);

    switch (fork()) {
    case -1:
        errExit("fork");
    case 0:
        /* child */
        if (close(parent_to_child_pipe_fd[1]) == -1) {
            errExit("close parent_to_child_pipe[1] - child");
        }
        
        if (close(child_to_parent_pipe_fd[0]) == -1) {
            errExit("clsoe child_to_parent_pipe[0] - child");
        }
        
        child_function(parent_to_child_pipe_fd[0], child_to_parent_pipe_fd[1]);
        
        _exit(EXIT_SUCCESS);
    default:
        /* parent */
        if (close(parent_to_child_pipe_fd[0]) == -1) {
            errExit("close parent_to_child_pipe[0] - parent");
        }
        
        if (close(child_to_parent_pipe_fd[1]) == -1) {
            errExit("clsoe child_to_parent_pipe[1] - parent");
        }
    
        parent_function(child_to_parent_pipe_fd[0], parent_to_child_pipe_fd[1]);
        exit(EXIT_SUCCESS);
    }
}


