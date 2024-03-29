#ifndef GET_NUM_H
#define GET_NUM_H

#define GN_NONNEG       01  /* value must be >= 0 */
#define GN_GT_0         02  /* value must be > 0 */

/* By default integers are decimal */
#define GN_ANY_BASE     0100    /* can use any base - like strtol(3) */
#define GN_BASE_8       0200    /* value is expresed in octal */
#define GN_BASE_16      0400    /* Value is expressed in hexadecimal */

long getLong(const char *arg, int flags, const char *name);
int getInt(const char *arg, int flags, const char *name);

#endif /* GET_NUM_H */ 
