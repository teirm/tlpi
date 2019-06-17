/*
 * A simplified implementation of popen
 *
 * 21 April 2019
 */

#include <unistd.h>
#include <stdio.h>

#define MAX_PROC    1024 

struct process_tracker_s {
    int length;
    pid_t process_array[MAX_PROC];
};
typedef struct process_tracker_s process_tracker_t;

FILE *my_popen(const char *command, const char *mode);
int my_pclose(FILE *stream);
