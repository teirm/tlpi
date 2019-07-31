/*
 * pmsg_receive.c
 *
 * Provided by TLPI
 */

#include <mqueue.h>
#include <fcntl.h>
#include <time.h>

#include <lib/tlpi_hdr.h>

static void
usageError(const char *progName)
{
    fprintf(stderr, "Usage: %s [-n] -t <time> name\n", progName);
    fprintf(stderr, "    -n          Use O_NONBLOCK flag\n");
    fprintf(stderr, "    -t <time>   Use a timeout\n");
    exit(EXIT_FAILURE);
}

static int
sendMessage(
    mqd_t          mqdes,
    char           *msg_ptr,
    size_t         msg_len,
    unsigned int   *msg_prio,
    const struct timespec *timeout)
{
    if (timeout != NULL) {
        return mq_timedreceive(mqdes, msg_ptr, msg_len, msg_prio, timeout);
    } else {
        return mq_receive(mqdes, msg_ptr, msg_len, msg_prio);
    }
}

int
main(int argc, char *argv[])
{
    int rc; 
    int flags;
    int opt;

    mqd_t mqd;
    unsigned int prio;
    void *buffer;
    struct mq_attr attr;
    struct timespec *tp = NULL;
    ssize_t numRead;
    
    char *tstr;
    long time_out_secs; 

    flags = O_RDONLY;
    while ((opt = getopt(argc, argv, "nt:")) != -1) {
        switch (opt) {
        case 'n':   flags |= O_NONBLOCK; break;
        case 't':   tstr = optarg; break;   
        default:    usageError(argv[0]);
        }
    }

    if (optind >= argc) {
        usageError(argv[0]);
    }
    
    if (tstr != NULL) {
        tp = malloc(sizeof(*tp));
        if (tp == NULL) {
            errExit("malloc");
        }
        
        time_out_secs = getLong(tstr, GN_NONNEG, "timeout");
        rc = clock_gettime(CLOCK_REALTIME, tp);
        if (rc == -1) {
            errExit("clock_gettime");
        }
        
        tp->tv_sec += time_out_secs;
    }

    mqd = mq_open(argv[optind], flags);
    if (mqd == (mqd_t) -1) {
        errExit("mq_open");
    }

    if (mq_getattr(mqd, &attr) == -1) {
        errExit("mq_getattr");
    }

    buffer = malloc(attr.mq_msgsize);
    if (buffer == NULL) {
        errExit("malloc");
    }

    numRead = sendMessage(mqd, buffer, attr.mq_msgsize, &prio, tp);
    if (numRead == -1) {
        errExit("sendMessage");
    }

    printf("Read %ld bytes; priority = %u\n", (long)numRead, prio);
    if (write(STDOUT_FILENO, buffer, numRead) == -1) {
        errExit("write");
    }
    write(STDOUT_FILENO, "\n", 1);

    exit(EXIT_SUCCESS);
}
