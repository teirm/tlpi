/*
 * Implementation of setenv and unsetenv using
 * putenv and getenv.
 *
 * 15 November 2018
 */

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <lib/tlpi_hdr.h>

extern char **environ;


int my_setenv(const char *name, const char *value, int overwrite)
{
    size_t name_len = 0;
    size_t value_len = 0;
    size_t buffer_size = 0;
    char *buf_ptr = NULL;
    char *env_buffer = NULL; 
    char *existence = getenv(name);

    if (existence && overwrite == 0) {
        return 0;
    }
    
    /* Total buffer: "name"="value"\0 hence the +2 for the \0
     * and =.
     */
    name_len = strlen(name);
    value_len = strlen(value);
    buffer_size = name_len + value_len + 2;
    
    env_buffer = malloc(buffer_size * sizeof(char));
    if (env_buffer == NULL) {
        errno = ENOMEM;
        return -1; 
    }
    /* Can't manipulate the pointer to the array.
     * Need to keep a pointer pointing to 
     * arrray position.
     */
    buf_ptr = env_buffer; 
    memcpy(buf_ptr, name, name_len);
    buf_ptr += name_len;
    *buf_ptr = '=';
    buf_ptr += 1;
    memcpy(buf_ptr, value, value_len);
    buf_ptr += value_len;
    *buf_ptr = '\0';
    
    if (putenv(env_buffer) != 0) {
        return -1;
    }

    return 0;
}

int my_unsetenv(const char *name)
{
    size_t name_len = 0; 
    char *putenv_buf = NULL;
    
    name_len = strlen(name);

    putenv_buf = malloc(name_len * sizeof(char));

    if (putenv_buf == NULL) {
        errno = ENOMEM;
        return -1;
    }
    
    /* abuse the glibc behavior of putenv -- not
     * sure if this is valid*/
    while (putenv(putenv_buf) == 0);
}

