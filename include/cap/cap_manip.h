/*
 * cap_manip.h
 *
 * capability manipulation wrapper functions.
 */

/* change setting of cap in caller's effective caps */
int modify_cap(int capability, int setting);

int raise_cap(int capability);

int drop_all_caps(void);
