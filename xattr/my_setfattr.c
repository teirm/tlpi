/*
 * my_setfattr.c
 *
 * A simplified version of the setfattr program.
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <sys/xattr.h>

int
main(int argc, char *argv[])
{
    int rc;
    size_t size;

    if (argc != 4 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "Usage: %s <name> <value> <path>\n", argv[0]);
        exit(EXIT_SUCCESS);
    }

    rc = setxattr(argv[3], argv[1], argv[2], strlen(argv[2]), 0);
    if (rc) {
        fprintf(stderr,
                "Error: unable to set xattr name=%s value=%s -- %s\n",
                argv[1],
                argv[2],
                strerror(errno));
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
