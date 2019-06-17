/*
 * my_realpath.c
 *
 * An implementation of realpath.
 */

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <libgen.h> 
#include <sys/stat.h>
#include <linux/limits.h>

#include <lib/tlpi_hdr.h>

static char*
my_realpath(const char *path, char *resolved_path);

static char *
handle_link(const char *path, char *resolved_path);

static char* handle_directory(const char *path, char *resolved_path);

static char*
handle_regular_file(const char *path, char *resolved_path);

static char*
handle_link(const char *path, char *resolved_path)
{
    struct stat stbuf;
    char link_buf[PATH_MAX];

    memset(link_buf, '\0', PATH_MAX);

    if (stat(path, &stbuf) == -1) {
        errMsg("stat");
        return NULL;
    }
    
    if (readlink(path, link_buf, PATH_MAX) == -1) {
        errMsg("readlink");
        return NULL;
    }


    switch (stbuf.st_mode & S_IFMT) {
    case S_IFDIR:
        handle_directory(link_buf, resolved_path);
    default:
        handle_regular_file(link_buf, resolved_path);
        break;
    }

    return resolved_path;
}

static char*
handle_directory(const char *path, char *resolved_path)
{
    if (chdir(path) == -1) {
        return NULL;
    }

    if (getcwd(resolved_path, PATH_MAX) == NULL)
        return NULL;

    return resolved_path;
}

static char*
handle_regular_file(const char *path, char *resolved_path)
{
    char *last_slash;
    char *base_name;
    char path_buf[PATH_MAX];
    memset(path_buf, '\0', PATH_MAX);

    memcpy(path_buf, path, strlen(path));
    
    last_slash = strrchr(path_buf, '/');
    if (last_slash == NULL) {
        base_name = path_buf;
    } else {
        *last_slash = '\0';
        base_name = last_slash + 1;
        if (chdir(path_buf) == -1) {
            errMsg("chdir\n");
            return NULL;
        }
    }


    if (getcwd(resolved_path, PATH_MAX) == NULL)
        errExit("getcwd");

    strncat(resolved_path, "/", strlen("/"));

    return strncat(resolved_path, base_name, strlen(base_name)); 
}


static char* 
my_realpath(const char *path, char *resolved_path)
{
    int rc;
    int curr_fd;
    
    struct stat stbuf;

    curr_fd = open(".", O_RDONLY);
    if (curr_fd == -1) {
        return NULL;
    }
    
    if (lstat(path, &stbuf) == -1) {
        return NULL;
    }
    
    switch (stbuf.st_mode & S_IFMT) {
        case S_IFDIR:
            handle_directory(path, resolved_path); 
            break;
        case S_IFLNK:
            printf("Handling link\n");
            handle_link(path, resolved_path);
            break;
        default:
            /* handle regular file */
            printf("handling regular file\n");
            handle_regular_file(path, resolved_path);
            break;
    }
    
    if (fchdir(curr_fd) == -1) {
        return NULL;
    }
    
    if (close(curr_fd) == -1) {
        return NULL;
    }

    return resolved_path;
}

int 
main(int argc, char *argv[])
{
    char path_buf[PATH_MAX]; 

    if (argc != 2) {
        usageErr("%s <path>\n", argv[0]);
    }
    
    if (my_realpath(argv[1], path_buf) == NULL) {
        errExit("my_realpath");
    }
    
    printf("Original Path: %s\n", argv[1]);
    printf("Resolved path: %s\n", path_buf);

    exit(EXIT_SUCCESS);
}
