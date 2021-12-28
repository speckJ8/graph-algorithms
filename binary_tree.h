#ifndef __BINARY_TREE__
#define __BINARY_TREE__

#include <stdlib.h>
#include <stdbool.h>
struct binary_tree_vertex_t {
    int label;
    int weight;
    struct binary_tree_vertex_t* parent;
    struct binary_tree_vertex_t* left;
    struct binary_tree_vertex_t* right;
    void* data;
};

typedef struct binary_tree_vertex_t binary_tree_vertex_t;
typedef binary_tree_vertex_t binary_tree_t;

/**
 * Perform a binary search for `value`.
 * */
binary_tree_t* bin_tree_find(binary_tree_t* tree, int value);

binary_tree_t* bin_tree_make(int root_label, int root_value);

/**
 * Determine whether a node is the left child of its parent node.
 * */
bool bin_tree_is_left(binary_tree_t* node);

/**
 * Determine whether a node is the right child of its parent node.
 * */
bool bin_tree_is_right(binary_tree_t* node);

/**
 * Free the memory of all the nodes in the tree. If there are fields
 * `data` allocated then they will also be free'd.
 * */
void bin_tree_free(binary_tree_t* tree);

binary_tree_t* bin_tree_insert(binary_tree_t* tree, int label, int value);

#endif // __BINARY_TREE__
