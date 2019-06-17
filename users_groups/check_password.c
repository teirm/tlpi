#define _DEFAULT_SOURCE
#define _XOPEN_SOURCE

#include <unistd.h>
#include <limits.h>
#include <pwd.h>
#include <shadow.h>

#include <lib/tlpi_hdr.h>

int 
main(int argc, char *argv[])
{
    char *username;
    char *password;
    char *encrypted;
    char *p;

    struct passwd *pwd;
    struct spwd *spwd;

    Boolean auth_ok;
    size_t len;
    long lnmax;

    lnmax = sysconf(_SC_LOGIN_NAME_MAX);
    if (lnmax == -1) 
        lnmax = 256;

    username = malloc(lnmax);
    if (username == NULL) 
        errExit("malloc");

    printf("Username: ");
    fflush(stdout);
    if (fgets(username, lnmax, stdin) == NULL) 
        exit(EXIT_FAILURE);

    len = strlen(username);
    if (username[len - 1] == '\n')
        username[len - 1] = '\0';

    pwd = getpwnam(username);
    if (pwd == NULL) 
        fatal("couldn't get password record");

    spwd = getspnam(username);
    if (spwd == NULL && errno == EACCES) 
        fatal("no permision to read shadow password file");

    if (spwd != NULL)
        pwd->pw_passwd = spwd->sp_pwdp;

    password = getpass("Password: ");

    encrypted = crypt(password, pwd->pw_passwd);
    for (p = password; *p != '\0'; )
            *p++ = '\0';
    
    if (encrypted == NULL)
        errExit("crypt");

    auth_ok = strcmp(encrypted, pwd->pw_passwd) == 0;
    if (!auth_ok) {
        printf("Incorrect password\n");
        exit(EXIT_FAILURE);
    }

    printf("Successfully authenticated: UID=%ld\n", (long) pwd->pw_uid);

    /* Now do authenticated work... */

    exit(EXIT_SUCCESS);
}
