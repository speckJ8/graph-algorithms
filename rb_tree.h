#ifndef __RB_TREE__
#define __RB_TREE__

#include "binary_tree.h"

enum rb_color_t {
    RED,
    BLACK,
};


struct rb_vertex_data_t {
    enum rb_color_t color;
};

typedef struct rb_vertex_data_t rb_vertex_data_t;
typedef binary_tree_vertex_t rb_vertex_t;
typedef binary_tree_t rb_tree_t;
typedef enum rb_color_t rb_color_t;

/**
 * Make a new tree.
 * */
rb_tree_t* rb_make(int root_label, int root_value);

/**
 * Frees `vertex` along with all of the vertices in its subtree.
 * */
void rb_free(rb_vertex_t* vertex);

/**
 * Make a new vertex and insert in the tree. Returns the new root of
 * the tree after it has been rebalanced.
 * */
rb_tree_t* rb_insert(rb_tree_t* tree, int label, int value);

rb_tree_t* rb_find(rb_tree_t* tree, int value);

/**
 * Print the tree.
 * */
void rb_show(binary_tree_t* tree);
void _rb_show(binary_tree_t* tree, int indent, char pos);

rb_tree_t* _rb_rebalance(rb_tree_t* tree);
void _rb_rotate_left(rb_tree_t* tree);
void _rb_rotate_right(rb_tree_t* tree);
void _rb_rotate(rb_tree_t* tree);
rb_color_t _rb_color(rb_tree_t* tree);
void _rb_set_color(rb_tree_t* tree, rb_color_t color);

#endif // __RB_TREE__
