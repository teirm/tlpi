/*
 * Verifying exercise 8.1
 */

#include <stdio.h>
#include <pwd.h>


int
main()
{
   printf("%s %s\n", getpwuid(996)->pw_name, getpwuid(1000)->pw_name);

   return 0;
}
