/*
 * t_mprotect.c
 *
 * Provided by TLPI
 */

#define _DEFAULT_SOURCE
#include <sys/mman.h>

#include <lib/tlpi_hdr.h>

#define LEN (1024 * 1024)

#define SHELL_FMT "cat /proc/%ld/maps | grep zero"
/* Allow extra space for integer string */
#define CMD_SIZE (sizeof(SHELL_FMT) + 20) 

int
main(int argc, char *argv[])
{
    char cmd[CMD_SIZE];
    char *addr;

    /* create an anonymous mapping with all access denied */

    addr= mmap(NULL, LEN, PROT_NONE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (addr == MAP_FAILED) {
        errExit("mmap");
    }

    /* display line from /proc/self/maps */

    printf("Before mprotect()\n");
    snprintf(cmd, CMD_SIZE, SHELL_FMT, (long)getpid());
    system(cmd);

    /* change protection on memory to allow read and write access */
    if (mprotect(addr, LEN, PROT_READ | PROT_WRITE) == -1) {
        errExit("mprotect");
    }

    printf("After mprotect()\n");
    system(cmd);

    exit(EXIT_SUCCESS);
}
