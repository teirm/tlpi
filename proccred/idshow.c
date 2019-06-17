#define _GNU_SOURCE
#include <unistd.h>
#include <sys/fsuid.h>
#include <limits.h>

#include <lib/tlpi_hdr.h>

#include "../users_groups/ugid_functions.h"

#define SG_SIZE (NGROUPS_MAX + 1)

int
main(int argc, char *argv[])
{
    uid_t ruid, euid, suid, fsuid;
    gid_t rgid, egid, sgid, fsgid;

    gid_t supp_groups[SG_SIZE];
    int num_groups, j;

    char *p;

    if (getresuid(&ruid, &euid, &suid) == -1)
        errExit("getresuid");
    if (getresgid(&rgid, &egid, &sgid) == -1)
        errExit("getresgid");

    /* Attempting to change the fs-ids are always ignored 
     * for unprivileged processes, but even so, the following 
     * calls return the current file-system ID.  This is 
     * a bug in the use of setfsuid / setfsgid functions because
     * they don't do any error checking.
     */

    fsuid = setfsuid(0);
    fsgid = setfsgid(0);

    printf("UID: ");
    p = userNameFromId(ruid);
    printf("real=%s (%ld); ", (p == NULL) ? "???" : p, (long) ruid);
    p = userNameFromId(euid);
    printf("effective=%s (%ld); ", (p == NULL) ? "???" : p, (long) euid);
    p = userNameFromId(suid);
    printf("saved=%s (%ld); ", (p == NULL) ? "???" : p, (long) suid);
    p = userNameFromId(fsuid);
    printf("fs=%s (%ld); ", (p == NULL) ? "???" : p, (long) fsuid);
    
    printf("GID: ");
    p = groupNameFromId(rgid);
    printf("real=%s (%ld); ", (p == NULL) ? "???" : p, (long) rgid);
    p = groupNameFromId(egid);
    printf("effective=%s (%ld); ", (p == NULL) ? "???" : p, (long) egid);
    p = groupNameFromId(sgid);
    printf("saved=%s (%ld); ", (p == NULL) ? "???" : p, (long) sgid);
    p = groupNameFromId(egid);
    printf("fs=%s (%ld); ", (p == NULL) ? "???" : p, (long) fsgid);
    
    num_groups = getgroups(SG_SIZE, supp_groups);
    if (num_groups== -1)
        errExit("getgroups");

    printf("Supplementary groups (%d): ", num_groups);
    for (j = 0; j < num_groups; j++) {
        p = groupNameFromId(supp_groups[j]);
        printf("%s (%ld); ", (p == NULL) ? "???" : p, (long) supp_groups[j]);
    }

    printf("\n");

    exit(EXIT_SUCCESS);
}
