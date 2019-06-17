/*
 * xattr_view.c
 *
 * A program to view all the extended
 * attributes of a file.
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include <sys/xattr.h>

#include <lib/tlpi_hdr.h>

#define XATTR_SIZE  10000

static void
usage_error(char *prog_name)
{
    fprintf(stderr, "Usage: %s [-x] file...\n", prog_name);
    exit(EXIT_FAILURE);
}

int
main(int argc, char *argv[])
{
    char list[XATTR_SIZE];
    char value[XATTR_SIZE];

    ssize_t list_len;
    ssize_t value_len;

    int ns;
    int j;
    int k;
    int opt;

    Boolean hex_display;

    hex_display = 0;
    while ((opt = getopt(argc, argv, "x")) != -1) {
        switch (opt) { 
        case 'x':
            hex_display = 1;
            break;
        case '?':
            usage_error(argv[0]);
        }
    }

    if (optind >= argc) {
        usage_error(argv[0]);
    }

    for (j = optind; j < argc; j++) {
        list_len = listxattr(argv[1], list, XATTR_SIZE);
        if (list_len == -1) {
            errExit("listxattr");
        }

        printf("%s:\n", argv[j]);

        /* loop through EA names displaying name + value */
        for (ns = 0; ns < list_len; ns += strlen(&list[ns]) + 1) {
            printf("        name=%s; ", &list[ns]);
            value_len = getxattr(argv[j], &list[ns], value, XATTR_SIZE);
            
            if (value_len == -1) {
                printf("couldn't get value");
            } else if (!hex_display) {
                printf("value=%.*s", (int)value_len, value);
            } else {
                printf("value=");
                for (k = 0; k < value_len; k++) {
                    printf("%02x ", (unsigned int) value[k]);
                }
            }

            printf("\n");
        }

        printf("\n");
    }
    exit(EXIT_SUCCESS);
}
