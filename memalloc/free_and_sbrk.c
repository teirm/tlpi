#define _DEFAULT_SOURCE

#include <lib/tlpi_hdr.h>

#define MAX_ALLOCS 1000000

int
main(int argc, char *argv[])
{
    char *ptr[MAX_ALLOCS];
    
    int free_step;
    int free_min;
    int free_max;
    
    int block_size;
    int num_allocs;
    int j;

    printf("\n");

    if (argc < 3 || strcmp(argv[1], "--help") == 0) {
        usageErr("%s num-allocs block-size [step] [min] [max]\n", argv[0]);
    }

    num_allocs = getInt(argv[1], GN_GT_0, "num-allocs");
    
    if (num_allocs > MAX_ALLOCS) {
        cmdLineErr("num-allocs > %d\n", MAX_ALLOCS);
    }

    block_size = getInt(argv[2], GN_GT_0 | GN_ANY_BASE, "block-size");

    free_step = (argc > 3) ? getInt(argv[3], GN_GT_0, "step") : 1;
    free_min = (argc > 4) ? getInt(argv[4], GN_GT_0, "min") : 1;
    free_max = (argc> 5) ? getInt(argv[5], GN_GT_0, "max") : num_allocs;

    if (free_max > num_allocs) {
        cmdLineErr("free_max > num_allocs\n");
    }

    printf("Initial program break:          %10p\n", sbrk(0));

    printf("Allocating %d*%d bytes\n", num_allocs, block_size);

    for (j = 0; j < num_allocs; j++) {
        ptr[j] = malloc(block_size);
        if (ptr[j] == NULL) {
            errExit("malloc");
        }
        printf("After malloc(%d) number %d\n program break is: %10p\n",
                block_size, j, sbrk(0));
    }

    printf("Program break is now:           %10p\n", sbrk(0));

    printf("Freeing blocks from %d to %d in steps of %d\n", 
                free_min, free_max, free_step);

    for (j = free_min - 1; j < free_max; j += free_step) {
        free(ptr[j]);
    }

    printf("After free(), program break is: %10p\n", sbrk(0));

    exit(EXIT_SUCCESS);
}
