/*
 * my_getcwd.c
 *
 * An implementation of getcwd.
 */

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>

#include <sys/stat.h>

#include <linux/limits.h>

static char *
scan_parent(dev_t device, ino_t inode)
{
    DIR *parent_dir;
    struct dirent *dp;

    struct stat stbuf;

    parent_dir = opendir(".");
    if (parent_dir == NULL) {
        return NULL;
    }

    errno = 0;
    while ((dp = readdir(parent_dir)) != NULL) {
        if (errno != 0) {
            return NULL;
        }

        if (stat(dp->d_name, &stbuf) == -1) {
            return NULL;
        }

        if (stbuf.st_dev == device && stbuf.st_ino == inode) {
            return dp->d_name;
        }
    }
    return NULL;
}

static char *
my_getcwd(char *buf, size_t size)
{
    int i; 
    int curr_fd;
    int path_index;
    size_t entry_len;
    dev_t curr_device; 
    ino_t curr_inode; 

    struct stat stbuf;
    char *path_entry;

    char *path_entries[1024];

    curr_fd = open(".", O_RDONLY);
    if (curr_fd == -1) {
        return NULL;
    }

    if (stat(".", &stbuf) == -1) {
        return NULL;
    }

    curr_device = stbuf.st_dev;
    curr_inode = stbuf.st_ino;

    if (stat("..", &stbuf) == -1) {
        return NULL;
    }
    
    path_index = 0;
    while (curr_device != stbuf.st_dev || curr_inode != stbuf.st_ino) {
        
        if (chdir("..") == -1) {
            break;
        }
        
        path_entries[path_index++] = scan_parent(curr_device, curr_inode);

        if (stat(".", &stbuf) == -1) {
            break;
        }
        
        curr_device = stbuf.st_dev;
        curr_inode = stbuf.st_ino;
    
        if (stat("..", &stbuf) == -1) {
            break;
        }
    }
    
    for (i = path_index-1; i >= 0; i--) {
        if (size - 1 < size || size - 1 == 0) {
            strncat(buf, "/", 1);
            size -= 1;
        } else {
            break;
        }
        
        entry_len = strlen(path_entries[i]);
        if (size - entry_len < size) {
            strncat(buf, path_entries[i], size - entry_len < size ? entry_len : size);
            size -= entry_len;
        } else {
            break;
        }
    }

    if (fchdir(curr_fd) == -1) {
        exit(EXIT_FAILURE); 
    }

    return buf;
}

int
main()
{
    char buf[PATH_MAX];
    if (my_getcwd(buf, PATH_MAX) == NULL) {
        exit(EXIT_FAILURE);
    }

    printf("%s\n", buf);

    exit(EXIT_SUCCESS);
}
