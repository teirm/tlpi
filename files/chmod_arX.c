/*
 * chmod_arX.c
 *
 * A small program to do chmod a+rX on a file.
 *
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#include <sys/stat.h>

static int chmod_arx(const char *name);

static int
chmod_arx(const char *name)
{
    int fd; 
    struct stat sb;
    
    if (stat(name, &sb) == -1) {
        return -1;
    }
    
    sb.st_mode |= (S_IRUSR | S_IRGRP | S_IROTH);

    if (S_ISDIR(sb.st_mode) || 
        (sb.st_mode & S_IXUSR) || 
        (sb.st_mode & S_IXGRP) ||
        (sb.st_mode & S_IXOTH)) {
        sb.st_mode |= (S_IXUSR | S_IXGRP | S_IXOTH);
    }
    
    if (chmod(name, sb.st_mode) == -1) {
        return -1;
    }

    return 0;
}

int
main(int argc, char *argv[]) 
{
    int i;
    int rc;
    int status = EXIT_SUCCESS;

    if (argc < 2) {
        fprintf(stderr, "Usage: chmod_arX <name_1> <name_2> ... <name_n>\n");
        exit(EXIT_FAILURE);
    }

    for (i = 1; i < argc; i++) {
        rc = chmod_arx(argv[i]);
        if (rc) {
            fprintf(stderr, "ERROR: Failed to chmod_arx %s -- %s\n",
                    argv[i], strerror(errno));
            status = EXIT_FAILURE;
        }
    }

    exit(status);
}
