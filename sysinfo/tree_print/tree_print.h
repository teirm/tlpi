/*
 * tree_print
 *
 * Generically print a tree data-structure in a manner showing
 * the parent child relationships.
 *
 * 2 December 2018
 */

#include <stdlib.h>
#include <stdio.h>

#define MAX_CHILDREN 100 

#define print_spaces(_s_)                   \
    do {                                    \
        int _i_;                            \
        for (_i_ = 0; _i_ < _s_; _i_++) {   \
            printf("   ");                  \
        }                                   \
    } while(0)                              \

/*
 * Tree Node structure.
 */
struct tree_node_s {
    int num_children;
    void *value;
    struct tree_node_s *children[MAX_CHILDREN];
};
typedef struct tree_node_s tree_node_t;

/**
 * @brief   Add a child node with the given to the tree 
 *          at the given parent
 *
 * @param[in]   parent      - parent to add child to
 * @param[in]   value       - void * representing the value
 * 
 * @return int
 */
int 
add_child(tree_node_t *parent, void *value);

/**
 * @brief   Find the first node in the tree satisfying the given comparator
 *          (Recursive DFS)
 *
 * @param[in]   root        - root of the tree
 * @param[in]   value       - value to search for
 * @param[in]   comparator  - comparison function defining search
 *                            criteria
 *
 * @return tree_node_t *                            
 */
tree_node_t* 
find_node(tree_node_t *root, void *value, int (*comparator)(void *a, void *b));

/**
 * @brief   Generically print a tree starting from the root.
 *
 * @param[in] root          - root of the (sub)tree
 * @param[in] print_node    - caller provided function to print a tree
 *                            node
 */
void 
print_tree(tree_node_t *root, void (*print_node)(tree_node_t *node));

/**
 * @brief   Destroy a tree an free any associated memory.
 *
 * @param[in]   root            - root of the tree
 * @param[in]   destroy_node    - caller provided function to free any memory
 *                                associated with the node
 */
void 
destroy_tree(tree_node_t *root, void (*destroy_node)(tree_node_t *node));

/**
 * @brief   Initialize a tree by creating and returning the root.
 * 
 * @param value     - value to store in root of the tree
 *
 * @return tree_node_t* 
 */
tree_node_t *init_tree(void *value);
