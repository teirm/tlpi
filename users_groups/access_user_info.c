/*
 * Accessing user information from C.
 *
 * 16 November 2018
 */

#include <pwd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

int 
main(int argc, char *argv[])
{
    struct passwd *user_info = NULL;
    
    if (argc != 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "%s <user-name>\n", argv[0]);
        exit(1);
    }
    
    errno = 0;
    user_info = getpwnam(argv[1]);
    if (user_info == NULL) {
        if (errno == 0) {
            fprintf(stderr, "user %s was not found found\n", argv[1]);
        } else {
            perror("Error");
        }
        exit(1);
    }

    printf("name: %s\nencrypted password: %s\nuid: %d\ngid: %d\ncomment: %s\nhome: %s\nshell: %s\n",
            user_info->pw_name,
            user_info->pw_passwd,
            user_info->pw_uid,
            user_info->pw_gid,
            user_info->pw_gecos,
            user_info->pw_dir,
            user_info->pw_shell);

    exit(EXIT_SUCCESS);
}

