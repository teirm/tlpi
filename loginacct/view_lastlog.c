/*
 * view_lastlog.c
 */

#include <time.h>
#include <lastlog.h>
#include <paths.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include <ugid/ugid_functions.h>
#include <lib/tlpi_hdr.h>

int main(int argc, char *argv[])
{
    struct lastlog llog;
    int fd;
    int j;
    uid_t uid;

    if (argc > 1 && strcmp(argv[1], "--help") == 0)
        usageErr("%s [username...]\n", argv[0]);

    fd = open(_PATH_LASTLOG, O_RDONLY);
    if (fd == -1)
        errExit("open");

    for (j = 1; j < argc; j++) {
        uid = userIdFromName(argv[j]);
        if (uid == -1) {
            printf("No such user: %s\n", argv[j]);
            continue;
        }

        if (lseek(fd, uid * sizeof(struct lastlog), SEEK_SET) == -1)
            errExit("lseek");

        if (read(fd, &llog, sizeof(struct lastlog)) <= 0) {
            printf("read failed for %s\n", argv[j]);
            continue;
        }

        printf("%p %s\n", &llog.ll_time, ctime((time_t *)&llog.ll_time)); 
        printf("%s\n", strerror(errno));
    
        time_t ll_time = llog.ll_time;
        printf("%-8.8s %-6.6s %-20.20s %s", argv[j], llog.ll_line,
                llog.ll_host, ctime(&ll_time));

    }

    close(fd);
    exit(EXIT_SUCCESS);
}
