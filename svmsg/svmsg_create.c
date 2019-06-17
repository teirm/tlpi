/*
 * svmsg_create.c
 */

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>

#include <lib/tlpi_hdr.h>

static void
usageError(const char *progName, const char *msg)
{
    if (msg != NULL) {
        fprintf(stderr, "%s", msg);
    }

    fprintf(stderr, "Usage: %s [-cx] {-f pathname | -k key | -p} "
                             "[octal-perms\n", progName);

    fprintf(stderr, "   -c              Use IPC_CREAT flag\n");
    fprintf(stderr, "   -x              Use IPC_EXCL flag\n");
    fprintf(stderr, "   -f pathname     Generate key using ftok()\n");
    fprintf(stderr, "   -k              Use 'key' as key\n");
    fprintf(stderr, "   -p              Use IPC_PRIVATE key\n");
    exit(EXIT_FAILURE);
}

int
main(int argc, char *argv[])
{
    /* counts -f, -k, and -p options */
    int num_key_flags;
    int flags;
    int msq_id;
    int perms;
    int opt;
    long lkey;
    key_t key;

    /* parse command-line options and arguments */
    num_key_flags = 0;
    flags = 0;

    while ((opt = getopt(argc, argv, "cf:k:px")) != -1) {
        switch (opt) {
        case 'c':
            flags |= IPC_CREAT;
            break;
        case 'f':
            key = ftok(optarg, 1);
            if (key == -1) {
                errExit("ftok");
            }
            num_key_flags++;
            break;
        case 'k':
            if (sscanf(optarg, "%li", &lkey) != 1) {
                cmdLineErr("-k option requires a numeric argument\n");
            }
            key = lkey;
            num_key_flags++;
            break;
        case 'p':
            key = IPC_PRIVATE;
            num_key_flags++;
            break;
        case 'x':
            flags |= IPC_EXCL;
            break;
        default:
            usageError(argv[0], "Bad option\n");
        }
    }

    if (num_key_flags != 1)
        usageError(argv[0], "Exactly one of the options -f, -k, "
                            "or -p must be supplied\n");
    
    perms = (optind == argc) ? (S_IRUSR | S_IWUSR) :
                getInt(argv[optind], GN_BASE_8, "octal-perms");

    msq_id = msgget(key, flags | perms);
    if (msq_id == -1) {
        errExit("msgget");
    }

    printf("%d\n", msq_id);
    exit(EXIT_SUCCESS);
}
