/*
 * Exercise 12-3 from The Linux Programming
 * Interface
 *
 * List all processes that have a particular path
 * name open.  The path name must be absolute.
 *
 * 25 November 2018
 */

#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>

#include <lib/tlpi_hdr.h>

static void
search_open_files(const char *pid_file_name, const char *path);

static void
search_procfs_files(DIR *procfs, const char *path);

static void
search_open_files(const char *pid_file_name, const char *path)
{
    DIR *pid_fd_dir = NULL; 
    struct dirent *curr_fd_link = NULL;

    char pid_fd_link_path[PATH_MAX];
    char fd_resolved_path[PATH_MAX]; 
    char pid_fd_dir_path[PATH_MAX] = "/proc/";

    strncat(pid_fd_dir_path, pid_file_name, strlen(pid_file_name));
    strncat(pid_fd_dir_path, "/fd/", strlen("/fd/"));

    pid_fd_dir = opendir(pid_fd_dir_path); 
    
    if (pid_fd_dir == NULL) {
        fprintf(stderr, "Unabled to open dir %s in %s\n", 
                pid_fd_dir, __func__);
        return;
    }

    while ((curr_fd_link = readdir(pid_fd_dir)) != NULL) {
        /* Need to zero out buffer since readlink does _not_
         * append the '\0' terminator on the end of the buffer
         */
        memset(fd_resolved_path, '\0', PATH_MAX); 
        strncpy(pid_fd_link_path, pid_fd_dir_path, strlen(pid_fd_dir_path)+1);
        strncat(pid_fd_link_path, curr_fd_link->d_name, strlen(curr_fd_link->d_name));

        readlink(pid_fd_link_path, fd_resolved_path, PATH_MAX);

        if (strcmp(fd_resolved_path, path) == 0) {
            printf("%s\n", pid_file_name);
        }
    }
    
    if (closedir(pid_fd_dir) == -1) {
        errExit("closedir in %s", __func__);
    }
    
    return;
}

static void
search_procfs_files(DIR *procfs, const char *path)
{
    struct dirent *curr_dir = NULL;

    if (path == NULL) {
        return;
    }

    if (*path == '\0') {
        return;
    }

    while ((curr_dir = readdir(procfs)) != NULL) {
        if (strtol(curr_dir->d_name, NULL, 10) != 0) {
            search_open_files(curr_dir->d_name, path); 
        }
    }

    return;
}

int
main(int argc, char *argv[])
{
    DIR *procfs = NULL;
    
    if (argc != 2 || strcmp(argv[1], "--help") == 0) {
        usageErr("%s <path>\n", argv[0]);
    }
    
    procfs = opendir("/proc/");
    if (procfs == NULL) {
        errExit("opendir");
    }

    search_procfs_files(procfs, argv[1]);
    
    if (closedir(procfs) == -1) {
        errExit("closedir");
    }
    
    exit(EXIT_SUCCESS);
}
