/*
 * A simple benchmark to time the creation 
 * of a large number of 1 byte files
 */

#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <linux/limits.h>

#define MIN_NUMBER 1000000
#define MAX_NUMBER 9999999 
#define MAX_LENGTH 100 

char**
init_name_array(const long size)
{
    int i;
    char **name_array = NULL;

    name_array = malloc(size * sizeof(char *));
    if (name_array == NULL) {
        return NULL; 
    }

    for (i = 0; i < size; i++) {
        name_array[i] = malloc(MAX_LENGTH);
        if (name_array[i] == NULL) {
            return NULL;
        }
    }
    
    return name_array;
}

void
destroy_name_array(char **name_array, const long size)
{
    int i;
    for (i = 0; i < size; i++) {
        free(name_array[i]);
        name_array[i] = NULL;
    }

    free(name_array);
    name_array = NULL;
}

void
generate_names(char **name_array, const long size)
{
    int i;
    for (i = 0; i < size; i++) {
        snprintf(name_array[i], MAX_LENGTH, "x%ld",
                (long)(random() + MIN_NUMBER));
    }
    
    return;
}

int
string_comparator(const void *a, const void *b)
{
    const char *a_str = (const char *)a;
    const char *b_str = (const char *)b;

    return strcmp(a_str, b_str);
}

void
sort_names(char **name_array, const long size)
{
    qsort(name_array, size, sizeof(char *), string_comparator);
}

void
create_files(char **name_array, const long size)
{
    int i;
    int fd;
    for (i = 0; i < size; i++) {
        if ((fd = open(name_array[i], O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR)) == -1) {
            fprintf(stderr, "ERROR: Unable to open file %s -- %s",
                    name_array[i], strerror(errno));
            exit(EXIT_FAILURE);
        }
        if (write(fd, "1", 1) == -1) {
            fprintf(stderr, "ERROR: Unable to write to file %s -- %s",
                    name_array[i], strerror(errno));
            exit(EXIT_FAILURE);
        }
        if (close(fd) == -1) {
            fprintf(stderr, "ERROR: failed to close file %s -- %s",
                    name_array[i], strerror(errno));
            exit(EXIT_FAILURE);
        }
    }
    return;
}

void
delete_files(char **name_array, const long size)
{
    int i;
    for (i = 0; i < size; i++) {
        if (unlink(name_array[i]) == -1) {
            fprintf(stderr, "ERROR: Unable to unlink file %s -- %s\n",
                    name_array[i], strerror(errno));
            exit(EXIT_FAILURE); 
        }
    }
    return;
}

int
main(int argc, char *argv[])
{
    long size;
    char *end_ptr = NULL;
    char curr_dir[PATH_MAX];
    char **name_array = NULL;

    time_t create_time_start;
    time_t create_time_end;
    time_t delete_time_start;
    time_t delete_time_end;


    if (argc != 3) {
        fprintf(stderr, "Usage: ./%s <nfiles> <target>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    size = strtol(argv[1], &end_ptr, 10);
    if (*end_ptr != '\0') {
        fprintf(stderr, "Unable to parse number of files: %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    if (mkdir(argv[2], S_IRUSR | S_IWUSR | S_IXUSR) == -1) {
        if (errno != EEXIST) {
            fprintf(stderr, "Unable to create target director: %s -- %s\n", 
                    argv[2], strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    if (getcwd(curr_dir, PATH_MAX) == NULL) {
        fprintf(stderr, "ERROR: Unable to get current working directory -- %s\n",
                strerror(errno));
        exit(EXIT_FAILURE); 
    }
    
    if (chdir(argv[2]) == -1) {
        fprintf(stderr, "ERROR: Unable to change into the target directory %s -- %s\n",
                argv[2], strerror(errno));
        exit(EXIT_FAILURE);
    }
    
    name_array = init_name_array(size);
    if (name_array == NULL) {
        fprintf(stderr, "ERROR: Unable to initialize name array.\n");
        exit(EXIT_FAILURE);
    }
    
    generate_names(name_array, size);
    create_time_start = time(NULL);
    create_files(name_array, size);
    create_time_end = time(NULL);
    
    sort_names(name_array, size);
    
    delete_time_start = time(NULL);
    delete_files(name_array, size);
    delete_time_end = time(NULL);
    destroy_name_array(name_array, size);

    if (chdir(curr_dir) == -1) {
        fprintf(stderr, "ERROR: Unable to return to working director %s -- %s\n",
                curr_dir, strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (rmdir(argv[2]) == -1) {
        fprintf(stderr, "ERROR: Unable to delete target directory: %s - %s\n", 
                argv[2], strerror(errno));
        exit(EXIT_FAILURE);
    }

    
    printf("Creation Time: %ld\tDeletion Time: %ld\n",
            create_time_end - create_time_start,
            delete_time_end - delete_time_start);


    exit(EXIT_SUCCESS);
}
