/*
 * nonreentrant.c
 *
 * non reentrant function in main
 * and signal handler
 *
 * 31 January 2019
 */

#define __XOPEN_SOURCE 600
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <lib/tlpi_hdr.h>

static char *str2;          /* set from argv[2] */
static int handled = 0;     /* count calls to handler */

static void
handler(int sig)
{
    crypt(str2, "xx");
    handled++;
}

int
main(int argc, char *argv[])
{
    char *cr1;
    int call_num;
    int mismatch;
    struct sigaction sa;

    if (argc != 3) {
        usageErr("%s str1 str2\n", argv[0]);
    }

    str2 = argv[2];
    cr1 = strdup(crypt(argv[1], "xx"));

    if (cr1 == NULL) {
        errExit("strdup");
    }

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = handler;

    if (sigaction(SIGINT, &sa, NULL) == -1) {
        errExit("sigaction");
    }

    /* repeated call crypt() using argv[1]. If interrupted by
     * a signal handler, then the static storage returned by
     * crypt() will be overwritten by results of encrypting 
     * argv[2], and strcmp() will detect a mismatch with the 
     * value in 'cr1'.
     */
    for (call_num = 1, mismatch = 0;  ; call_num++) {
        if (strcmp(crypt(argv[1], "xx"), cr1) != 0) {
            mismatch++;
            printf("Mismatch on call %d (mismatch=%d handled=%d)\n",
                    call_num, mismatch, handled);
        }
    }
}
