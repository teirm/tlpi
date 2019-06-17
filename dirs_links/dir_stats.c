/*
 * dir_stats.c
 *
 * A program that uses nftw to
 * collect statistics on a directory
 * tree.
 */

#define _XOPEN_SOURCE 600
#include <stdio.h>
#include <stdlib.h>
#include <ftw.h>
#include <string.h>
#include <errno.h>

#include <sys/stat.h>

struct dir_stats {
    unsigned int total;
    unsigned int n_dir;
    unsigned int n_file;
    unsigned int n_char;
    unsigned int n_blk;
    unsigned int n_lnk;
    unsigned int n_fifo;
    unsigned int n_sock;
} stats;

static void
process_stats()
{
    if (stats.total != 0) {
        printf("directories:    %u  (%.2lf%%)\n", stats.n_dir, 100 * (double)stats.n_dir / stats.total);
        printf("files:          %u  (%.2lf%%)\n", stats.n_file,100 * (double)stats.n_file / stats.total);
        printf("char_dev:       %u  (%.2lf%%)\n", stats.n_char, 100 * (double)stats.n_char / stats.total);
        printf("block_dev:      %u  (%.2lf%%)\n", stats.n_blk, 100 * (double)stats.n_blk / stats.total);
        printf("link:           %u  (%.2lf%%)\n", stats.n_lnk, 100 *(double)stats.n_lnk / stats.total);
        printf("fifo:           %u  (%.2lf%%)\n", stats.n_fifo, 100 * (double)stats.n_fifo / stats.total);
        printf("socket:         %u  (%.2lf%%)\n", stats.n_sock, 100 * (double)stats.n_sock / stats.total);
    }
    printf("total:          %u\n", stats.total);
}


static int
dir_stats(const char *pathname, const struct stat *sbuf, int type, struct FTW *ftwb)
{
    switch (sbuf->st_mode & S_IFMT) {
    case S_IFREG:  stats.n_file++; break;
    case S_IFDIR:  stats.n_dir++;  break;
    case S_IFCHR:  stats.n_char++; break;
    case S_IFBLK:  stats.n_blk++;  break;
    case S_IFLNK:  stats.n_lnk++;  break;
    case S_IFIFO:  stats.n_fifo++; break;
    case S_IFSOCK: stats.n_sock++; break;
    }

    stats.total++;

    return 0;
}

int 
main(int argc, char *argv[])
{
    if (argc != 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "%s <dir>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    if (nftw(argv[1], dir_stats, 20, 0) == -1) {
        perror("nftw");
        exit(EXIT_FAILURE);
    }
    
    process_stats();
    exit(EXIT_SUCCESS);
}





