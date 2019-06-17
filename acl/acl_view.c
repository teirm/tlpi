/*
 * acl_view.c 
 *
 * Provided by tlpi
 */

#include <acl/libacl.h>
#include <sys/acl.h>

#include <ugid/ugid_functions.h>
#include <lib/tlpi_hdr.h>

static void
usageError(char *progName) 
{
    fprintf(stderr, "Usage: %s [-d] filename\n", progName);
    exit(EXIT_FAILURE);
}

int
main(int argc, char *argv[])
{
    int entry_id;
    int perm_val;
    int opt;
    
    char* name;
    uid_t *uidp;
    gid_t *gidp;
    
    acl_t acl;
    acl_type_t type;
    acl_entry_t entry;
    acl_tag_t tag;
    acl_permset_t permset;

    type = ACL_TYPE_ACCESS;
    while ((opt = getopt(argc, argv, "d")) != -1) {
        switch (opt) {
            case 'd': 
                type = ACL_TYPE_DEFAULT;
                break;
            case '?':
                usageError(argv[0]);
                break;
        }
    }

    if (optind + 1 != argc) {
        usageError(argv[0]);
    }

    acl = acl_get_file(argv[optind], type);
    if (acl == NULL) {
        errExit("acl_get_file");
    }
    
    /* walk through each entry in the acl */
    for (entry_id = ACL_FIRST_ENTRY; ; entry_id = ACL_NEXT_ENTRY) {
        if (acl_get_entry(acl, entry_id, &entry) != 1) {
            break;
        }
        
        /* retrieve and display tag type */
        if (acl_get_tag_type(entry, &tag) == -1) {
            errExit("acl_get_trag_type");
        }

        printf("%-12s", (tag == ACL_USER_OBJ) ?  "user_obj" :
                        (tag == ACL_USER) ?      "user" :
                        (tag == ACL_GROUP_OBJ) ? "group_obj" :
                        (tag == ACL_GROUP) ?     "group" :
                        (tag == ACL_MASK) ?      "mask" :
                        (tag == ACL_OTHER) ?     "other" : "???");

        /* retrieve and display optional tag qualifiers */
        if (tag == ACL_USER) {
            uidp = acl_get_qualifier(entry);
            if (uidp == NULL) {
                errExit("acl_get_qualifier");
            }

            name = userNameFromId(*uidp);
            if (name == NULL) {
                printf("%-8d ", *uidp);
            } else {
                printf("%-8s ", name);
            }

            if (acl_free(uidp) == -1) {
                errExit("acl_free");
            }
        } else if (tag == ACL_GROUP) {
            gidp = acl_get_qualifier(entry);
            if (gidp == NULL) {
                errExit("acl_get_qualifier");
            }
            
            name = groupNameFromId(*gidp);
            if (name == NULL) {
                printf("%-8d ", *gidp);
            } else {
                printf("%-8s ", name);
            }

            if (acl_free(gidp) == -1) {
                errExit("acl_free");
            }
        } else {
            printf("         ");
        }

        /* retrieve and display information */
        if (acl_get_permset(entry, &permset) == -1) {
            errExit("acl_get_permset");
        }

        perm_val = acl_get_perm(permset, ACL_READ);
        if (perm_val == -1) {
            errExit("acl_get_perm - ACL_READ");
        }
        printf("%c", (perm_val == 1) ? 'r' : '-');
    
        perm_val = acl_get_perm(permset, ACL_WRITE);
        if (perm_val == -1) {
            errExit("acl_get_perm - ACL_WRITE");
        }
        printf("%c", (perm_val == 1) ? 'w' : '-');
        
        perm_val = acl_get_perm(permset, ACL_EXECUTE);
        if (perm_val == -1) {
            errExit("acl_get_perm - ACL_EXECUTE");
        }
        printf("%c", (perm_val == 1) ? 'x' : '-');
        
        printf("\n");
    }

    if (acl_free(acl) == -1) { 
        errExit("acl_free");
    }

    exit(EXIT_SUCCESS);
}
