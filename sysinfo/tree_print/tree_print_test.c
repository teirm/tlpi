/*
 * tree_print_test
 *
 * Test the tree print library by creating a tree
 * of integers.
 *
 * 2 December 2018
 */

#include <stdio.h>
#include "tree_print.h"

static int 
comparator(void *a, void *b)
{
    return *(int *)a == *(int *)b;
}

static void 
destroy_node(tree_node_t *node)
{
    printf("Destroying node %p with value %d\n", 
            node, *(int *)node->value);
}

static void
print_node(tree_node_t *node) 
{
    printf("Node Value: %d\n", *(int *)node->value);
}


/*
 * A static test for tree_print
 *
 * Construct the following tree:
 *  0
 *  |
 *  *--1
 *  |  |
 *  |  *--4
 *  |
 *  |--2
 *  |  |
 *  |  *--5
 *  |     |
 *  |     *-- 6
 *  |--3
 *  |  |
 *  |  *--7
 *  |  |  |
 *  |  |  *--8
 *  |  |
 *  |  *--9
 *  |  
 *  *--10
 */

int
main()
{
    tree_node_t *root   = NULL;
    tree_node_t *parent = NULL;
    int test_array[]    = {0,1,2,3,4,5,6,7,8,9,10};
    
    root = init_tree(&test_array[0]);
    
    if (root == NULL) {
        fprintf(stderr, "failed to initialize tree\n");
        exit(EXIT_FAILURE);
    }
    
    /* Create children off root 0 */
    parent = find_node(root, &test_array[0], comparator);
    if (parent == NULL) {
        fprintf(stderr, "failed to find root after insertion of value 0\n");
        exit(EXIT_FAILURE);
    }
    
    if (add_child(parent, &test_array[1])) {
        fprintf(stderr, "failed to insert child with value 1\n");
        exit(EXIT_FAILURE);
    }
    
    if (add_child(parent, &test_array[2])) {
        fprintf(stderr, "failed to insert child with value 2\n");
        exit(EXIT_FAILURE);
    }
    
    if (add_child(parent, &test_array[3])) {
        fprintf(stderr, "failed to insert child with value 3\n");
        exit(EXIT_FAILURE);
    }
    
    if (add_child(parent, &test_array[10])) {
        fprintf(stderr, "failed to insert child with value 10\n");
        exit(EXIT_FAILURE);
    }

    /* Create children off root 1 */
    parent = find_node(root, &test_array[1], comparator);
    if (parent == NULL) {
        fprintf(stderr, "failed to find node with value 1\n");
        exit(EXIT_FAILURE);
    }

    if (add_child(parent, &test_array[4])) {
        fprintf(stderr, "failed to insert child with value 4\n");
        exit(EXIT_FAILURE);
    }
    
    /* Create children off root 2 */
    parent = find_node(root, &test_array[2], comparator);
    if (parent == NULL) {
        fprintf(stderr, "failed to find node with value 1\n");
        exit(EXIT_FAILURE);
    }

    if (add_child(parent, &test_array[5])) {
        fprintf(stderr, "failed to insert child with value 4\n");
        exit(EXIT_FAILURE);
    }

    /* Create children off root 3 */
    parent = find_node(root, &test_array[3], comparator);
    if (add_child(parent, &test_array[7])) {
        fprintf(stderr, "failed to insert child with value 4\n");
        exit(EXIT_FAILURE);
    }
    
    if (add_child(parent, &test_array[9])) {
        fprintf(stderr, "failed to insert child with value 4\n");
        exit(EXIT_FAILURE);
    }
        
    /* Create children off root 5 */
    parent = find_node(root, &test_array[5], comparator);
    if (add_child(parent, &test_array[6])) {
        fprintf(stderr, "failed to insert child with value 4\n");
        exit(EXIT_FAILURE);
    }
    
    /* Create children off root 7 */
    parent = find_node(root, &test_array[7], comparator);
    if (add_child(parent, &test_array[8])) {
        fprintf(stderr, "failed to insert child with value 4\n");
        exit(EXIT_FAILURE);
    }

    print_tree(root, print_node);

    destroy_tree(root, destroy_node);

    exit(EXIT_SUCCESS);
}
