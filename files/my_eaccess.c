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
#include <errno.h>

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
    

