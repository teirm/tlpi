/*
 * cap_manip.c 
 *
 * capability manipulation wrapper functions.
 */

#define _DEFAULT_SOURCE
#define _XOPEN_SOURCE
#include <stdlib.h>
#include <sys/capability.h>

#include <cap/cap_manip.h>

/* change setting of cap in caller's effective caps */
int modify_cap(int capability, int setting)
{
    cap_t caps;
    cap_value_t cap_list[1];

    /* retrieve caller's current capabilities */
    caps = cap_get_proc();
    if (caps == NULL) {
        return -1;
    }

    /* change setting of 'capability' in effective set of 'caps'.
     * The third argument, 1, is the number of items in array 'cap_list'
     */
    cap_list[0] = capability;
    if (cap_set_flag(caps, CAP_EFFECTIVE, 1, cap_list, setting) == -1) {
        cap_free(caps);
        return -1;
    }

    /* push modified caps to kernel */
    if (cap_set_proc(caps) == -1) {
        cap_free(caps);
        return -1;
    }

    /* free the struture that was allcoated by libcap */
    if (cap_free(caps) == -1) {
        return -1;
    }

    return 0;
}

int raise_cap(int capability)
{
    return modify_cap(capability, CAP_SET);
}

int drop_all_caps(void)
{
    cap_t empty;
    int s;

    empty = cap_init();
    if (empty == NULL) {
        return -1;
    }

    s = cap_set_proc(empty);
    if (cap_free(empty) == -1) {
        return -1;
    }

    return s;
}
#undef _XOPEN_SOURCE
#undef _DEFAULT_SOURCE
