#include <stdio.h> 
#include <errno.h>
#include <stdlib.h>

#include <sys/time.h>

int main()
{
    int rc = 0; 
    struct timeval tv;

    rc = gettimeofday(&tv, NULL);

    if (rc != 0) {
        perror("Error");
        exit(EXIT_FAILURE);
    }

    printf("%ld %ld\n", (long) tv.tv_sec, (long) tv.tv_usec);
    
    exit(EXIT_SUCCESS);
}
