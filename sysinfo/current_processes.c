/*
 * Exercise 12-1 from The Linux Programming Interface
 *
 * Displays all processes being run by the user named
 * in the program's command line.
 *
 * 25 November 2018
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <fcntl.h>
#include <pwd.h>
#include <ctype.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <lib/tlpi_hdr.h>

/* BUFFER_SIZE CALCULATION:
 *   Maximum entry size + 
 *   "/proc/" (6) + 
 *   "/status" (7)
 *   + "\0" (1)
 */
#define BUF_SIZE (PATH_MAX + 14)

#define MAX_LINE 50


/* Return UID corresponding to name */

static uid_t
user_id_from_name(const char *name);

static void
search_procfs_uid(DIR *procfs, uid_t uid);

static void
read_process_status(const char *d_name, uid_t uid);

static uid_t 
user_id_from_name(const char *name)
{
    struct passwd *pwd = NULL; 

    if (name == NULL) {
        return -1;
    }

    if (*name == '\0') {
        return -1;
    }

    pwd = getpwnam(name);
    if (pwd == NULL) {
        return -1;
    }

    return pwd->pw_uid;
}

static void
read_process_status(const char *d_name, uid_t uid)
{
    uid_t process_uid;
    pid_t pid;
    FILE *status_file = NULL; 
    
    char label[MAX_LINE];
    char command_name[MAX_LINE];
    char abs_proc_path[BUF_SIZE] = "/proc/";
    
    strncat(abs_proc_path, d_name, strlen(d_name));
    strncat(abs_proc_path, "/status", strlen("/status"));
    
    status_file = fopen(abs_proc_path, "r");
    if (status_file == NULL) {
        printf("status file was NULL");
        return;
    }
    
    while (fscanf(status_file, "%s:", label) != EOF) {
        if (strcmp(label, "Name:") == 0) {
            fscanf(status_file, "%s", command_name);
        } else if (strcmp(label, "Pid:") == 0) {
            fscanf(status_file, "%ld", (long *) &pid);
        } else if (strcmp(label, "Uid:") == 0) {
            fscanf(status_file, "%ld", (long *) &process_uid);
        }
    }

    if (fclose(status_file) == EOF) {
        fprintf(stderr, "Error closing status_file\n");
    }

    if (process_uid == uid) {
        printf("Command: %s\nPid: %d\n", command_name, pid);
    }
}


static void 
search_procfs_uid(DIR *procfs, uid_t uid)
{
    struct dirent *curr_dir = NULL;

    while ((curr_dir = readdir(procfs)) != NULL) {
        if (strtol(curr_dir->d_name, NULL, 10) != 0) {
            read_process_status(curr_dir->d_name, uid);
        }
    }
}

int
main(int argc, char *argv[])
{
    uid_t uid; 
    DIR *procfs = NULL;

    if (argc != 2 || strcmp("--help", argv[1]) == 0) {
        usageErr("%s <user_name>\n", argv[0]); 
    }
    
    uid = user_id_from_name(argv[1]);
    if (uid == -1) {
        errExit("user_name %s is invalid\n", uid);
    }

    procfs = opendir("/proc");
    if (procfs == NULL) {
        errExit("opendir");
    }

    search_procfs_uid(procfs, uid);

    if (closedir(procfs) == -1) {
        errExit("closedir");
    }

    exit(EXIT_SUCCESS);
}
