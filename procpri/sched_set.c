/*
 * sched_set.c
 *
 * Provided by TLPI
 */

#include <sched.h>
#include <lib/tlpi_hdr.h>

int main(int argc, char *argv[])
{
    int j;
    int pol;
    struct sched_param sp;

    if (argc < 3 || strchr("rfobi", argv[1][0]) == NULL)
        usageErr("%s policy priority [pid...]\n"
                 "   policy is 'r' (RR), 'f' (FIFO, "
#ifdef SCHED_BATCH
                 "'b' (BATCH),"
#endif /* SCHED_BATCH */
#ifdef SCHED_IDLE
                 "'i' (IDLE), "
#endif /* SCHED_IDLE */
                 "or 'o' (OTHER)\n",
                 argv[0]);
    pol = (argv[1][0] == 'r') ? SCHED_RR :
                (argv[1][0] == 'f') ? SCHED_FIFO :
#ifdef SCHED_BATCH
                (argv[1][0] == 'b') ? SCHED_BATCH :
#endif /* SCHED_BATCH */
#if SCHED_IDLE
                (argv[1][0] == 'i') ? SCHED_IDLE :
#endif /* SCHED_IDLE */
                SCHED_OTHER;
    sp.sched_priority = getInt(argv[2], 0, "priority");

    for (j = 3; j < argc; j++)
        if (sched_setscheduler(getLong(argv[j], 0, "pid"), pol, &sp) == -1)
            errExit("sched_setscheduler");

    exit(EXIT_SUCCESS);
}
