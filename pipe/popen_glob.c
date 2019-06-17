/*
 * popen_glob.c
 *
 * Provided by TLPI
 */

#include <ctype.h>
#include <limits.h>

#include <procexec/print_wait_status.h>
#include <lib/tlpi_hdr.h>

#define POPEN_FMT       "/bin/ls -d %s 2> /dev/null"
#define PAT_SIZE        50
#define PCMD_BUF_SIZE   (sizeof(POPEN_FMT) + PAT_SIZE)

int main(int argc, char *argv[])
{
    char pat[PAT_SIZE];
    char popen_cmd[PCMD_BUF_SIZE];
    FILE *fp;
    Boolean bad_pattern;
    
    int len;
    int status;
    int file_count;
    int j;
    char pathname[PATH_MAX];

    for (;;) {
        printf("pattern: ");
        fflush(stdout);
        if (fgets(pat, PAT_SIZE, stdin) == NULL)
            break;
        len = strlen(pat);
        if (len <= 1)
            continue;

        if (pat[len - 1] == '\n')
            pat[len - 1] = '\0';

        /* Ensure only valid characters in pattern */

        for (j = 0, bad_pattern = FALSE; j < len && !bad_pattern; j++) {
            if (!isalnum((unsigned char) pat[j]) && strchr("_*?[^-].", pat[j]) == NULL) {
                bad_pattern = TRUE;
            }
        }

        if (bad_pattern) {
            printf("Bad pattern character: %c\n", pat[j - 1]);
            continue;
        }

        /* build and execute command to glob 'pat' */
        snprintf(popen_cmd, PCMD_BUF_SIZE, POPEN_FMT, pat);

        fp = popen(popen_cmd, "r");
        if (fp == NULL) {
            printf("popen() failed\n");
            continue;
        }

        /* read resulting list of pathnames until EOF */
        file_count = 0;
        while(fgets(pathname, PATH_MAX, fp) != NULL) {
            printf("%s", pathname);
            file_count++;
        }

        /* close pipe -- fetch and display termination status */

        status = pclose(fp);
        printf("    %d matching file %s\n", file_count, (file_count != 1) ? "s" : "");
        printf("    pclose() status = %#x\n", (unsigned int) status);
        if (status != -1)
            printWaitStatus("\t", status);
    }
    exit(EXIT_SUCCESS);
}


