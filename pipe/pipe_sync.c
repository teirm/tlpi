/*
 * pipe_sync.c
 */

#include <time/curr_time.h>
#include <lib/tlpi_hdr.h>

int main(int argc, char *argv[])
{
    int pfd[2];
    int j;
    int dummy;

    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        usageErr("%s sleep-time...\n", argv[0]);
    }
    
    /* make stdout unbuffered since we terminate child 
     * with _exit 
     */
    setbuf(stdout, NULL);
    
    printf("%s  Parent started\n", currTime("%T"));

    if (pipe(pfd) == -1) {
        errExit("pipe");
    }

    for (j = 1; j < argc; j++) {
        switch (fork()) {
        case -1:
            errExit("fork %d", j);
        case 0:
            /* child */
            if (close(pfd[0]) == -1)
                errExit("close");

            /* 
             * child does some work and lets 
             * parent know when it is done.
             */

            sleep(getInt(argv[j], GN_NONNEG, "sleep-time"));
            printf("%s  Child %d (PID=%ld) closing pipe\n",
                    currTime("%T"), j, (long)getpid());
            if(close(pfd[1]) == -1)
                errExit("close");

            _exit(EXIT_SUCCESS);
        default:
            break;
        }
    }

    /* 
     * Parent comes here; close write end of pipe so we can see
     * EOF
     */

    if (close(pfd[1]) == -1)
        errExit("close");
    
    
    /* parent may do other work then synchronize with children */
    if (read(pfd[0], &dummy, 1) != 0)
        fatal("parent didn't get EOF");
    
    printf("%s  Parent ready to go\n", currTime("%T"));

    /* parent can now carry on to do other things... */
    
    exit(EXIT_SUCCESS);
}
