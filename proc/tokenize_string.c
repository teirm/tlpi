/*
 * Tokeninzing a string of the form 
 *  "name=value"
 * 
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
    char str[] = "PATH=/home";
    char *name;
    char *value;

    name = strtok(str, "=");
    value = strtok(NULL, "=");
    
    if (strcmp(name, "PATH") == 0) {
        printf("MATCH\n");
    } else {
        printf("No match\n");
    }

    printf("name: %s\tvalue: %s\n", name, value);
    
    exit(EXIT_SUCCESS);
}
