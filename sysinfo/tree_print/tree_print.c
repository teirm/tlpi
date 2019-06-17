/*
 * tree_print
 *
 * Generically print a tree data-structure in a manner showing
 * the parent child relationships.
 *
 * 2 December 2018
 */

#include "tree_print.h"

static void 
print_tree_level(tree_node_t *root, void (*print_node)(tree_node_t *node), int level);

/**
 * @brief   Find the first node in the tree satisfying the given comparator
 *          (Recursive DFS)
 *
 * @note    Can eliminate use of recursion with a stack
 *
 * @param[in]   root        - root of the (sub)tree
 * @param[in]   value       - value to search for
 * @param[in]   comparator  - comparison function defining search
 *                            criteria
 *
 * @return int                            
 */
tree_node_t*
find_node(tree_node_t *root, void *value, int (*comparator)(void *a, void *b))
{
    int curr_child;
    tree_node_t *search_result = NULL; 

    if (comparator(root->value, value)) {
        return root;
    }

    for (curr_child = 0; curr_child < root->num_children; curr_child++) {
        search_result = find_node(root->children[curr_child], value, comparator);

        if (search_result != NULL) {
            return search_result;
        }
    }

    return search_result;
}

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
add_child(tree_node_t *parent, void *value)
{
    int current_children = 0; 
    tree_node_t *child_node = NULL; 

    if (parent == NULL) {
        return -1;
    }
    current_children = parent->num_children;

    if (current_children == MAX_CHILDREN) {
        return -1;
    }
    
    child_node = malloc(sizeof(tree_node_t));
    
    if (child_node == NULL) {
        return -1;
    }
   
    child_node->value = value;
    child_node->num_children = 0;

    parent->children[current_children] = child_node;
    parent->num_children++;
    
    return 0;
}

/**
 * @brief   Destroy a (sub)tree and free any associated memory.
 *
 * @param[in]   root            - root of the tree
 * @param[in]   destroy_node    - caller provided function to free any memory
 *                                associated with the node
 */
void 
destroy_tree(tree_node_t *root, void (*destroy_node)(tree_node_t *node))
{
    int i;
    tree_node_t *curr_child = NULL;

    if (root == NULL) {
        return;
    }

    for (i = 0; i < root->num_children; i++) {
        curr_child = root->children[i];
        destroy_tree(curr_child, destroy_node);
    }

    if (destroy_node != NULL) {
        destroy_node(root);
    }

    free(root);
    root = NULL;

    return;
}

/**
 * @brief   Initialize a tree by creating and returning the root.
 *
 * @param value     - value to store in root of the tree
 *
 * @return tree_node_t* 
 */
tree_node_t *
init_tree(void *value) 
{
    tree_node_t *new_tree = NULL;

    new_tree = malloc(sizeof(tree_node_t));
    
    if (new_tree == NULL) {
        return NULL;
    }

    new_tree->num_children = 0;
    new_tree->value = value;
    
    return new_tree;
}

/**
 * @brief   Generically print a tree starting from the root.
 *
 * @param[in] root          - root of the (sub)tree
 * @param[in] print_node    - caller provided function to print a tree
 *                            node
 */
void 
print_tree(tree_node_t *root, void (*print_node)(tree_node_t *node))
{
    print_tree_level(root, print_node, 0);
}

/**
 * @brief   Generically print a tree strating from the root while
 *          tree level.
 *          
 * @param[in] root          - root of the (sub)tree
 * @param[in] print_node    - caller provided function to print a tree node
 * @param[in] level         - level in the tree
 */
static void 
print_tree_level(tree_node_t *root, void (*print_node)(tree_node_t *node), int level)
{
    int i; 
    
    /* Indent parent 'level' spaces */ 
    print_spaces(level);    
    /* Print the parent */
    print_node(root);

    for (i = 0; i < root->num_children; i++) {
        print_tree_level(root->children[i], print_node, level+1);
    }
    
    return;
}





