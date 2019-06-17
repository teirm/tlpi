/*
 * acl_perms.c
 *
 * Exercise from ch 17
 */

#include <stdlib.h>
#include <stdio.h>

#include <acl/libacl.h>
#include <sys/acl.h>

#include <lib/tlpi_hdr.h>
#include <ugid/ugid_functions.h>


static void
mask_permissions(acl_permset_t *req_permset, acl_permset_t *mask_permset)
{
    int req_read;
    int req_write;
    int req_exec;

    int mask_read;
    int mask_write;
    int mask_exec;

    req_read = acl_get_perm(*req_permset, ACL_READ);
    if (req_read == -1) {
        errExit("acl_get_perm -- req_permset, ACL_READ");
    }

    mask_read = acl_get_perm(*mask_permset, ACL_READ);
    if (mask_read == -1) {
        errExit("acl_get_perm -- mask_permset, ACL_READ");
    }

    req_write = acl_get_perm(*req_permset, ACL_WRITE);
    if (req_write == -1) {
        errExit("acl_get_perm -- req_permset, ACL_WRITE");
    }

    mask_write = acl_get_perm(*mask_permset, ACL_WRITE);
    if (mask_write == -1) {
        errExit("acl_get_perm -- mask_permset, ACL_WRITE");
    }
    
    req_exec = acl_get_perm(*req_permset, ACL_EXECUTE);
    if (req_exec == -1) {
        errExit("acl_get_perm -- req_permset, ACL_EXECUTE");
    }

    mask_exec = acl_get_perm(*mask_permset, ACL_EXECUTE);
    if (mask_exec == -1) {
        errExit("acl_get_perm -- mask_permset, ACL_EXECUTE");
    }
    
    printf("%c", (req_read && mask_read) ? 'r' : '-');
    printf("%c", (req_write && mask_write) ? 'w': '-');
    printf("%c", (req_exec && mask_exec) ? 'x' : '-');

    printf("\n");

    return;
}


int
main(int argc, char *argv[])
{
    int opt;
    int want_uid = 0;
    int want_gid = 0;
    int uid_match = 0;
    int gid_match = 0;
    int entry_id;
    uid_t uid;
    gid_t gid;
    uid_t *ace_uidp;
    gid_t *ace_gidp;

    char *filename = NULL;
    char *endptr;

    acl_t acl;
    acl_type_t type;
    acl_entry_t ace;
    acl_tag_t tag;

    acl_permset_t mask_permset;
    acl_permset_t req_permset;

    while ((opt = getopt(argc, argv, "u:g:")) != -1) {
        switch (opt) {
            case 'u':
                if ((uid = userIdFromName(optarg)) == -1) {
                    uid = strtol(optarg, &endptr, 10);
                    if (*endptr == '\0') {
                        errExit("Invalid uid %s\n", optarg);
                    }
                }
                want_uid = 1;
                break;
            case 'g':
                if ((gid = groupIdFromName(optarg)) == -1) {
                    gid = strtol(optarg, &endptr, 10);
                    if (*endptr == '\0') {
                        errExit("Invalid gid %s\n", optarg);
                    }
                }
                want_gid = 1;
                break;
            default: /* ? */
                errExit("Invalid input %c\n", opt);
                break;
        }
    }

    if (optind + 1 != argc) {
        usageErr("%s [u user | g group] file", argv[0]);
    }

    type = ACL_TYPE_ACCESS;
    acl = acl_get_file(argv[optind], type);
    if (acl == NULL) {
        errExit("acl_get_file");
    }

    for (entry_id = ACL_FIRST_ENTRY; ; entry_id = ACL_NEXT_ENTRY) {
        if (acl_get_entry(acl, entry_id, &ace) != 1) {
            break;
        }

        if (acl_get_tag_type(ace, &tag) == -1) {
            errExit("acl_get_tag_type");
        }

        if (want_uid && tag == ACL_USER) {
            ace_uidp = acl_get_qualifier(ace);
            if (ace_uidp == NULL) {
                errExit("acl_get_qualifier");
            }
            
            if (uid == *ace_uidp) {
                if (acl_get_permset(ace, &req_permset) == -1) {
                    errExit("acl_get_permset");
                }
            }

            uid_match = 1;
            acl_free(ace_uidp);
        }

        if (want_gid && tag == ACL_GROUP) {
            ace_gidp = acl_get_qualifier(ace);
            if (ace_gidp == NULL) {
                errExit("acl_get_qualifier");
            }

            if (gid == *ace_gidp) {
                if (acl_get_permset(ace, &req_permset) == -1) {
                    errExit("acl_get_permset");
                }
            }

            gid_match = 1;
            acl_free(ace_gidp);
        }

        if (tag == ACL_MASK) {
            if (acl_get_permset(ace, &mask_permset) == -1) {
                errExit("acl_get_permset");
            }
        }
    }

    if (uid_match || gid_match) {
        printf("%u: ", uid_match ? uid : gid);
        mask_permissions(&req_permset, &mask_permset);
    }

    exit(EXIT_SUCCESS);
}
