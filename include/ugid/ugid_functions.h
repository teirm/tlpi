/*
 * Functions to convert user and group IDs
 * to and from user and group names.
 *
 * 17 November 2018
 */

#include <pwd.h>
#include <grp.h>

char *userNameFromId(uid_t uid);
uid_t userIdFromName(const char *name);
char *groupNameFromId(gid_t gid);
gid_t groupIdFromName(const char *name);
