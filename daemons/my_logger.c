/*
 * A logger like clone
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <syslog.h>

int main(int argc, char *argv[])
{
    int priority;

    if (argc > 3 || (argc == 2 && strcmp(argv[1], "--help") == 0)) {
        fprintf(stderr, "./my_logger [priority] msg\n");
        exit(EXIT_SUCCESS);
    }

    openlog(NULL, LOG_PID, LOG_LOCAL0);
    
    if (argc == 3) { 
        priority = atoi(argv[2]);
    } else {
        priority = LOG_INFO;
    }

    syslog(priority, "%s", argv[argc - 1]);

    closelog();

    exit(EXIT_SUCCESS);
}
