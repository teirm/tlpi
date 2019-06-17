/*
 * Problem 9-3 from TLPI
 *
 * 23 November 2018
 *
 */

#include <grp.h>
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

#include <sys/types.h>

#ifdef DEBUG
#include <stdio.h>
#include <errno.h>
#endif

#define SG_SIZE (NGROUPS_MAX + 1)

int my_initgroups(const char *user, gid_t group)
{
    int rc = 0; 
    size_t i = 0; 
    gid_t supp_groups[SG_SIZE];
    struct group *grp = NULL;
    char **grp_list = NULL;
    char *gr_member = NULL;

    if (user == NULL)
        return -1;

    if (*user == '\0')
        return -1;
    
    while ((grp = getgrent()) != NULL) {
        grp_list = grp->gr_mem;
#ifdef DEBUG
        printf("Group Name: %s\n", grp->gr_name);
#endif 
        while ((gr_member = *grp_list) != NULL) {
            if (strcmp(gr_member, user) == 0) {
#ifdef DEBUG
                printf("Group ID: %u\n", grp->gr_gid);
#endif 
                supp_groups[i] = grp->gr_gid;
                i++;
            }
            grp_list++;
        }
    }
    
    if (i != SG_SIZE) {
        supp_groups[i++] = group;
    }
    
    rc = setgroups(i, supp_groups);

    if (rc == -1) {
        return -1;
    }

    return 0;
}

/*
 * Driver program.
 */
int
main()
{
    int rc = 0;
    char *user = "teirm";

    gid_t group = 99;

    rc = my_initgroups(user, group);

#ifdef DEBUG
    if (rc != 0) {
        printf("rc: %d\n", rc);
        perror("ERROR");
    }
#endif

    return rc;
}
