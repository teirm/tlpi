/*
 * utmpx_login.c
 */

#define _GNU_SOURCE
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <utmpx.h>
#include <paths.h>
#include <string.h>
#include <errno.h>
#include <lastlog.h>

#include <lib/tlpi_hdr.h>
#include <ugid/ugid_functions.h>

static int update_lastlog(struct utmpx *ut)
{
    int fd;
    uid_t uid;
    struct lastlog llog;

    if (ut == NULL) {
        return -1;
    }

    fd = open(_PATH_LASTLOG, O_RDWR);
    if (fd == -1) {
        return -1;
    }
    
    uid = userIdFromName(ut->ut_user);
    if (uid == -1) {
        return -1;
    }

    if (lseek(fd, uid * sizeof(struct lastlog), SEEK_SET) <= 0) {
        return -1;
    }

    llog.ll_time = ut->ut_tv.tv_sec;
    strncpy(llog.ll_line, ut->ut_line, sizeof(ut->ut_line));
    strncpy(llog.ll_host, "", sizeof(""));

    if (write(fd, &llog, sizeof(struct lastlog)) <= 0) {
        return -1;
    }

    if (close(fd) == -1) {
        return -1;
    }
}


int main(int argc, char *argv[])
{
    int rc;
    struct utmpx ut;
    char *dev_name;
    if (argc < 2 || strcmp(argv[1], "--help") == 0)
        usageErr("%s username [sleep-time]\n", argv[0]);

    /* init loginc record for utmp and wtmp files */

    memset(&ut, 0, sizeof(struct utmpx));
    ut.ut_type = USER_PROCESS;
    strncpy(ut.ut_user, argv[1], sizeof(ut.ut_user));
    if (time((time_t *) &ut.ut_tv.tv_sec) == -1)
        errExit("time");
    ut.ut_pid = getpid();

    /* set ut_line and ut_id based on the terminal associated with
     * 'stdin'.  Assume terminals named "/dev/[pt]t[sy]*".
     * The "/dev/" name is 5 characters; the "[pt]t[sy]" filename
     * prefix is 3 characters (making 8 characters in all).
     */
    dev_name = ttyname(STDIN_FILENO);
    if (dev_name == NULL)
        errExit("ttypname");
    if (strlen(dev_name) <= 8)
        fatal("Terminal name is too short: %s", dev_name);

    strncpy(ut.ut_line, dev_name + 5, sizeof(ut.ut_line));
    strncpy(ut.ut_id, dev_name + 8, sizeof(ut.ut_id));

    printf("creating login entries in utmp and wtmp\n");
    printf("        using pid %ld, line %.*s, id %.*s\n",
            (long) ut.ut_pid, (int) sizeof(ut.ut_line), ut.ut_line,
            (int) sizeof(ut.ut_id), ut.ut_id);

    /* rewind to start of utmp file */ 
    setutxent();

    /* write login record to utmp */
    if (pututxline(&ut) == NULL)
        errExit("pututxline");
    updwtmpx(_PATH_WTMP, &ut);

    rc = update_lastlog(&ut);
    if (rc) {
        fprintf(stderr, "ERROR: failed to update last log -- %s\n", strerror(errno));
    }

    /* sleep a while, so we can examine utmp and wtmp files */
    sleep((argc > 2) ? getInt(argv[2], GN_NONNEG, "sleep-time") : 15);

    /* now do a "lgout"; use values from previously initialized 'ut', 
     * except for changes below */

    ut.ut_type = DEAD_PROCESS;
    time((time_t *) &ut.ut_tv.tv_sec);
    memset(&ut.ut_user, 0, sizeof(ut.ut_user));

    printf("creating logout entries in utmp and wtmp\n");
    setutxent();
    if (pututxline(&ut) == NULL)
        errExit("pututxline");
    updwtmpx(_PATH_WTMP, &ut);

    endutxent();
    exit(EXIT_SUCCESS);
}


