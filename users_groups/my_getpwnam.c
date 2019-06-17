/* 
 * Implementation of getpwnam
 * using setpwent(), getpwent(), and endpwent();
 * 
 * 17 November 2018
 */

#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct passwd *
my_getpwnam(const char *name)
{
    static struct passwd pwd;
    struct passwd *p = NULL;
   
    if (name == NULL || *name == '\0') {
        return NULL;
    }

    while ((p = getpwent()) != NULL) {
        if (strcmp(name, p->pw_name) == 0) {
            pwd = *p;
            break;
        }
    }
    
    endpwent();

    return &pwd;
}

int 
main()
{
    struct passwd *pwd = NULL;

    pwd = my_getpwnam("teirm");
    
    if (pwd) {
        printf("UID: %d\n", pwd->pw_uid);
    } else {
        printf("NULL pwd\n");
        exit(-1);
    }

    exit(EXIT_SUCCESS);
}
