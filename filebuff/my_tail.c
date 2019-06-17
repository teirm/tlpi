/*
 * my_tail.c
 *
 * A simplified implementation of the shell
 * utility "tail" that prints out the last 
 * n lines of the given file
 *
 * 05 January 2018
 */

#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <lib/tlpi_hdr.h>

#define DEFAULT_LINE_COUNT 10

static int 
scan_newlines(char *buf, unsigned int buf_size, int *required);

static long
get_bufsize(struct stat *stat_buf);

static int 
scan_newlines(char *buf, unsigned int buf_size, int *required)
{
    int i;
    int lines_found = 0;
    for (i = buf_size - 1; i >= 0; i--) {
        if (buf[i] == '\n') {
            lines_found++;
        }
        if (lines_found == *required) {
            break;
        }
    }
    
    *required -= lines_found;
    return i;
}

static long
get_bufsize(struct stat *stat_buf)
{
    /* determine buffer size and offset size 
     * based on file size (st_size) */
    if (stat_buf->st_size <= 4096) {
        return 4096;
    } else if (stat_buf->st_size > 4096 && stat_buf->st_size <= 8192) {
        return 8192;
    } else {
        return 16384;
    }
}

int
main(int argc, char *argv[])
{
    int fd;
    int lines;
    off_t pos; 
    off_t offset;
    ssize_t count;
    char *buf;
    long buf_size;
    struct stat stat_buf;

    if (argc < 2 || argc > 4 || strcmp(argv[1], "--help") == 0) {
        usageErr("my_tail [-n num] file\n");
    }

    if (argc == 4) {
        lines = getLong(argv[2], GN_ANY_BASE, "lines") + 1;
        fd = open(argv[3], O_RDONLY);
        if (fd == -1) {
            errExit("open");
        }
    } else {
        lines = DEFAULT_LINE_COUNT + 1;
        fd = open(argv[1], O_RDONLY);
        if (fd == -1) {
            errExit("open");
        }
    }
    
    if (fstat(fd, &stat_buf) == -1) {
        errExit("fstat");
    }

    buf_size = get_bufsize(&stat_buf);

    buf = malloc(sizeof(char) * buf_size);
    if (buf == NULL) {
        errExit("malloc");
    }
    
    if ((offset = lseek(fd, (-1) * buf_size, SEEK_END)) == -1) {
        errExit("lseek");
    }

    while (offset != 0) {
        if (read(fd, buf, buf_size) == -1) {
            errExit("read");
        }
       
        pos = scan_newlines(buf, buf_size, &lines);

        if (lines == 0) {
            break;
        }
        
        if ((offset = lseek(fd, (-2) * buf_size, SEEK_CUR)) == -1) {
            errExit("lseek");
        }
    }
    
    /* Need to +1 to skip last found '\n' */
    if (offset != 0) {
        if (write(STDOUT_FILENO, buf + pos + 1, (buf_size - pos - 1)) == -1) {
            errExit("write");
        }
    }

    memset(buf, '\0', buf_size);
    while ((count = read(fd, buf, buf_size)) > 0) {
        write(STDOUT_FILENO, buf, buf_size);
        memset(buf, '\0', buf_size);
    }
    
    free(buf); 

    if (close(fd) == -1) {
        errExit("close");
    }

    exit(EXIT_SUCCESS);
}
