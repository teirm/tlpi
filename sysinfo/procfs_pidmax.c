#include <fcntl.h>
#include <lib/tlpi_hdr.h>

#define MAX_LINE 100

int 
main(int argc, char *argv[])
{
    int fd;
    char line[MAX_LINE];

    ssize_t n;

    fd = open("/proc/sys/kernel/pid_max", (argc > 1) ? O_RDWR : O_RDONLY);
    if (fd == -1)
        errExit("open");

    n = read(fd, line, MAX_LINE);
    if (n == -1)
        errExit("read");

    if (argc > 1)
        printf("Old value: ");
    printf("%.*s", (int) n, line);

    if (argc > 1) {
        if (lseek(fd, 0, SEEK_SET) == -1)
            errExit("lseek");

        if (write(fd, argv[1], strlen(argv[1])) != strlen(argv[1]))
            fatal("write() failed");
        
        if (lseek(fd, 0, SEEK_SET) == -1)
            errExit("lseek");

        n = read(fd, line, MAX_LINE);
        if (n == -1)
            errExit("read");

        printf("/proc/sys/kernel/pid_max now contains %.*s\n",
                (int) n, line);
    }

    exit(EXIT_SUCCESS);
}
