/*
 * A simplified implementation of popen
 *
 * 21 April 2019
 */

#include <string.h>
#include <errno.h>
#include <stdlib.h>

#include <sys/wait.h>

#include "my_popen.h"

#define MAX_BUFFER_LEN      256
#define MAX_COMMAND_LEN     128
#define COMMAND_TEMPLATE    "/bin/sh %s"

enum popen_modes {
    PROCESS_READ = 0,
    PROCESS_WRITE,
    PROCESS_INVALID
};

static process_tracker_t popen_array;

static int add_process(FILE *stream, pid_t pid);
static pid_t get_process(FILE *stream);
static int remove_process(FILE *stream);

static int stream_to_fd(FILE *stream);
static int get_mode(const char *mode);

static void child_worker(const char *command, int *pipe_fd, int process_mode);
static FILE *parent_worker(pid_t child_pid, int *pipe_fd, int process_mode);

static int stream_to_fd(FILE *stream)
{
    int fd;
    fd = fileno(stream);

    if (fd == -1) {
        fprintf(stderr, "ERROR: unable to get file descriptor -- %s\n",
                strerror(errno));
        return -1;
    }

    return fd;
}

static int add_process(FILE *stream, pid_t pid)
{
    int fd;
    
    fd = stream_to_fd(stream);
    if (fd == -1) {
        return -1;
    }

    if (fd > MAX_PROC) {
        fprintf(stderr, "ERROR: file descriptor is too large %d\n",
                fd);
        return -1;
    }

    if (popen_array.process_array[fd] != 0) {
        fprintf(stderr, "ERROR: process is already tracked\n");
        return -1;
    }

    popen_array.process_array[fd] = pid;
    popen_array.length++;
    return 0;
}

static pid_t get_process(FILE *stream)
{
    int fd;
    pid_t pid;

    fd = stream_to_fd(stream);
    if (fd == -1) {
        return -1;
    }
    
    if (fd > MAX_PROC) {
        fprintf(stderr, "ERROR: file descriptor is too large %d\n",
                fd);
        return -1;
    }

    pid = popen_array.process_array[fd];
    if (pid == 0) {
        fprintf(stderr, "ERROR: process is not tracked\n");
        return -1;
    }

    return pid;
}

static int remove_process(FILE *stream)
{
    int fd;

    fd = stream_to_fd(stream);
    if (fd == -1) {
        return -1;
    }

    if (fd > MAX_PROC) {
        fprintf(stderr, "ERROR: file descriptor is too large %d\n",
                fd);
        return -1;
    }
    
    if (popen_array.process_array[fd] == 0) {
        fprintf(stderr, "ERROR: process is not tracked\n");
        return -1;
    }

    popen_array.process_array[fd] = 0;
    popen_array.length--;
    return 0;
}

static int get_mode(const char *mode) {
    if (strcmp(mode, "w") == 0) {
        return PROCESS_WRITE;
    } else if (strcmp(mode, "r") == 0) {
        return PROCESS_READ;
    } else {
        return PROCESS_INVALID;
    }
}

static void child_worker(const char *command, int *pipe_fd, int process_mode)
{
    int rc;

    if (process_mode == PROCESS_READ) {
        close(pipe_fd[0]);
        dup2(pipe_fd[1], STDOUT_FILENO);

    }

    if (process_mode == PROCESS_WRITE) {
        close(pipe_fd[1]);
        dup2(pipe_fd[0], STDIN_FILENO);
    }
   
    rc = execl("/bin/sh", "sh", "-c", command, (char *)0);  
    
    if (rc == -1) {
        fprintf(stderr, "Error: Unable to exec shell -- %s\n",
                strerror(errno));
    }
    exit(EXIT_FAILURE);
}

static FILE *parent_worker(pid_t child_pid, int *pipe_fd, int process_mode)
{
    int rc;
    int used_fd;
    FILE *pipe_file = NULL; 

    if (process_mode == PROCESS_READ) {
        close(pipe_fd[1]);
        used_fd = pipe_fd[0];
        pipe_file = fdopen(used_fd, "r");
    }

    if (process_mode == PROCESS_WRITE) {
        close(pipe_fd[0]);
        used_fd = pipe_fd[1];
        pipe_file = fdopen(used_fd, "w");
    }

    rc = add_process(pipe_file, child_pid);
    if (rc == -1) {
        fclose(pipe_file);
        close(used_fd); 
        return NULL; 
    }

    return pipe_file;
}

FILE *my_popen(const char *command, const char *mode) 
{
    int rc;
    int process_mode;
    int pipe_fd[2];
    pid_t child_pid; 

    /*
     * 1) create pipe
     * 2) fork a child
     * 3) depending on mode pass end of pipe to parent
     *    as FILE*
     * 4) depending on mode dup2 a the other end of pipe
     * 5) exec command
     */
    process_mode = get_mode(mode);
    if (process_mode == PROCESS_INVALID) {
        return NULL;
    }
    
    rc = pipe(pipe_fd);
    if (rc == -1) {
        return NULL;
    }
   
    switch (child_pid = fork()) {
    case -1:
        /* error */
        break;
    case 0:
        child_worker(command, pipe_fd, process_mode); 
        break;
    default:
        return parent_worker(child_pid, pipe_fd, process_mode); 

    }
    return NULL;
}

int my_pclose(FILE *stream)
{
    int wstatus; 
    int fd;
    pid_t pid;
   
    if (stream == NULL) {
        return -1;
    }

    pid = get_process(stream);
    if (pid == -1) {
        return -1;
    }

    if (waitpid(pid, &wstatus, 0) == -1) {
        fprintf(stderr, "ERROR: failed to wait for child -- %s\n",
                strerror(errno));
        return -1;
    }
    
    fd = stream_to_fd(stream);
    if (close(fd) == -1) {
        fprintf(stderr, "ERROR: unable to close pipe -- %s\n",
                strerror(errno));
        return -1;
    }

    return wstatus;
}

int main()
{
    int rc; 
    char read_buffer[MAX_BUFFER_LEN];      
    
    FILE *list_file;
    FILE *echo_file;

    list_file = my_popen("/usr/bin/ls", "r");
    if (list_file == NULL) {
        exit(EXIT_FAILURE);
    }
    
    memset(read_buffer, 0, MAX_BUFFER_LEN);
    while (fscanf(list_file, "%s", read_buffer) != EOF) {
        printf("%s\n", read_buffer);
        memset(read_buffer, 0, MAX_BUFFER_LEN);
    }

    rc = my_pclose(list_file);
    if (rc == -1) {
        fprintf(stderr, "ERROR: failed to close\n");
        exit(EXIT_FAILURE);
    }

    echo_file = my_popen("/usr/bin/cat", "w");
    if (echo_file == NULL) {
        exit(EXIT_FAILURE);
    }

    fprintf(echo_file, "foobar\n");
    fprintf(echo_file, "foobar\n");
    fflush(echo_file);    
    rc = my_pclose(echo_file);
    if (rc == -1) {
        fprintf(stderr, "ERROR: failed to close\n");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
