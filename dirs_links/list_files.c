/*
 * list_files.c
 *
 * provided by dirs_links
 */

#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>

#include <lib/tlpi_hdr.h>

static void
list_files(const char *dirpath)
{
    DIR *dirp;
    struct dirent entry;
    struct dirent *result;
    Boolean is_current;

    is_current = strcmp(dirpath, ".") == 0;

    dirp = opendir(dirpath);
    if (dirp == NULL) {
        errMsg("opendir failed on '%s'", dirpath);
        return;
    }

    /* for each entry in the directory, print directory + filename */
    for (;;) {
        errno = readdir_r(dirp, &entry, &result);
        if (errno > 0)
            break;

        if (strcmp(entry.d_name, ".") == 0 || strcmp(entry.d_name, "..") == 0)
            continue;

        if (!is_current)
            printf("%s/", dirpath);
        printf("%s\n", entry.d_name);
    }
    
    if (errno != 0)
        errExit("readdir");
    
    if (closedir(dirp) == -1)
        errMsg("closedir");
}

int
main(int argc, char *argv[])
{
    if (argc > 1 && strcmp(argv[1], "--help") == 0)
        usageErr("%s [dir...]\n", argv[0]);

    if (argc == 1)
        list_files(".");
    else
        for (argv++; *argv; argv++)
            list_files(*argv);

    exit(EXIT_SUCCESS);
}
