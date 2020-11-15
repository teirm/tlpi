/*
 * my_eaccess.c
 *
 * An implementation of eaccess that
 * works by calling access after changing
 * the real uid and gid to the euid egid.
 * Changes are reverted on success or failure
 *
 */

#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>

#include <linux/limits.h>

int 
my_eaccess(const char *pathname, int mode)
{
    int rc; 
     
    uid_t orig_uid;
    gid_t orig_gid;
       
    orig_uid = getuid();
    orig_gid = getgid();

    rc = setuid(geteuid());
    rc = setgid(getegid());

    rc = access(pathname, mode);

    rc = setuid(orig_uid);
    rc = setuid(orig_gid);

    return 0;
}

int
main(int argc, char *argv[])
{
    const char *test_file = "test_file.txt";
    int fd = open(test_file, O_CREAT);
    if (fd == -1) {
        fprintf(stderr, "failure to create test file '%s'\n", test_file);
    }
    
    my_eaccess(test_file, R_OK);
    
    unlink(test_file);
}
