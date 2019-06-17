/*
 * demo_inotify.c
 *
 * Provided by TLPI
 */

#include <limits.h>
#include <sys/inotify.h>

#include <lib/tlpi_hdr.h>

#define BUF_LEN (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))

/* display information from inotify_event structure */
static void
display_inotify_event(struct inotify_event *i)
{
    printf("    wd =%d; ", i->wd);
    if (i->cookie > 0) {
        printf("cookie =%4d; ", i->cookie);
    }

    printf("mask = ");
    if (i->mask & IN_ACCESS)        printf("IN_ACCESS ");
    if (i->mask & IN_ATTRIB)        printf("IN_ATTRIB ");
    if (i->mask & IN_CLOSE_NOWRITE) printf("IN_CLOSE_NOWRITE ");
    if (i->mask & IN_CLOSE_WRITE)   printf("IN_CLOSE_WRITE ");
    if (i->mask & IN_CREATE)        printf("IN_CREATE ");
    if (i->mask & IN_DELETE)        printf("IN_DELETE ");
    if (i->mask & IN_DELETE_SELF)   printf("IN_DELTE_SELF ");
    if (i->mask & IN_IGNORED)       printf("IN_IGNORED ");
    if (i->mask & IN_ISDIR)         printf("IN_ISDIR ");
    if (i->mask & IN_MODIFY)        printf("IN_MODIFY ");
    if (i->mask & IN_MOVE_SELF)     printf("IN_MOVE_SELF ");
    if (i->mask & IN_MOVED_FROM)    printf("IN_MOVED_FROM ");
    if (i->mask & IN_MOVED_TO)      printf("IN_MOVED_TO ");
    if (i->mask & IN_OPEN)          printf("IN_OPEN");
    if (i->mask & IN_Q_OVERFLOW)    printf("IN_Q_OVERFLOW");
    if (i->mask & IN_UNMOUNT)       printf("IN_UNMOUNT");
    printf("\n");

    if (i->len > 0) {
        printf("        name = %s\n", i->name);
    }
}

int
main(int argc, char *argv[])
{
    int inotify_fd;
    int wd;
    int j;

    char buf[BUF_LEN] __attribute__((aligned(8)));
    ssize_t num_read;
    char *p;
    struct inotify_event *event;

    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        usageErr("%s pathname... \n", argv[0]);
    }

    inotify_fd = inotify_init();
    if (inotify_fd == -1) {
        errExit("inotify_init");
    }

    for (j = 1; j < argc; j++) {
        wd = inotify_add_watch(inotify_fd, argv[j], IN_ALL_EVENTS);
        if (wd == -1) {
            errExit("inotify_add_watch");
        }

        printf("watching %s using wd %d\n", argv[j], wd);
    }

    for (;;) {
        num_read = read(inotify_fd, buf, BUF_LEN);
        if (num_read == 0) {
            fatal("read() from inotify fd returned 0!");
        }

        if (num_read == -1) {
            errExit("read");
        }

        printf("read %ld bytes form inotify fd\n", (long)num_read);

        /* process all of the events in buffer returned by read() */

        for (p = buf; p < buf + num_read; ) {
            event = (struct inotify_event *)p;
            display_inotify_event(event);

            p += sizeof(struct inotify_event) + event->len;
        }
    }

    exit(EXIT_SUCCESS);
}
