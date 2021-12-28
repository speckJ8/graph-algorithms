#include "rb_tree.h"
#include <stdlib.h>
#include <stdio.h>

rb_tree_t* rb_make(int root_label, int root_value) {
    rb_vertex_t* vertex = bin_tree_make(root_label, root_value);
    vertex->data = malloc(sizeof(rb_vertex_data_t));
    _rb_set_color(vertex, BLACK);
    return vertex;
}

void rb_free(rb_tree_t* tree) {
    // `bin_tree_free` will also free the `data` allocations
    bin_tree_free(tree);
}

rb_tree_t* rb_insert(rb_tree_t* tree, int label, int value) {
    rb_vertex_t* vertex = bin_tree_insert(tree, label, value);
    vertex->data = malloc(sizeof(rb_vertex_data_t));
    _rb_set_color(vertex, RED);
    if (_rb_color(vertex->parent) == RED) {
        rb_tree_t* new_root = _rb_rebalance(vertex);
        if (new_root != NULL) {
            return new_root;
        }
    }
    return tree;
}

rb_tree_t* rb_find(rb_tree_t* tree, int value) {
    return bin_tree_find(tree, value);
}

void rb_show(binary_tree_t* tree) {
    _rb_show(tree, 0, '*');
}

rb_tree_t* _rb_rebalance(rb_tree_t* tree) {
    rb_tree_t* target = tree;
    while (true) {
        if ((bin_tree_is_left(target) && bin_tree_is_left(target->parent)) ||
                (bin_tree_is_right(target) && bin_tree_is_right(target->parent))) {
            _rb_set_color(target, BLACK);
            target = target->parent;
        } else {
            rb_tree_t* next_target = target->parent;
            _rb_rotate(target);
            target = next_target;
            continue;
        }

        _rb_rotate(target);

        if (target->parent == NULL) {
            _rb_set_color(target, BLACK);
            return target;
        }
        if (_rb_color(target->parent) == BLACK &&
                (target->left == NULL || _rb_color(target->left) == BLACK) &&
                (target->right == NULL || _rb_color(target->right) == BLACK)) {
            break;
        }
    }
    return NULL;
}

void _rb_rotate_left(rb_tree_t* tree) {
    rb_tree_t* parent = tree->parent;
    rb_tree_t* grandparent = tree->parent->parent;
    rb_tree_t* left_child = tree->left;

    parent->right = left_child;
    if (left_child != NULL) {
        left_child->parent = parent;
    }
    tree->left = parent;
    if (grandparent != NULL) {
        if (bin_tree_is_left(parent)) {
            grandparent->left = tree;
        } else {
            grandparent->right = tree;
        }
    }
    parent->parent = tree;
    tree->parent = grandparent;
}

void _rb_rotate_right(rb_tree_t* tree) {
    rb_tree_t* parent = tree->parent;
    rb_tree_t* grandparent = tree->parent->parent;
    rb_tree_t* right_child = tree->right;

    parent->left = right_child;
    if (right_child != NULL) {
        right_child->parent = parent;
    }
    tree->right = parent;
    if (grandparent != NULL) {
        if (bin_tree_is_left(parent)) {
            grandparent->left = tree;
        } else {
            grandparent->right = tree;
        }
    }
    parent->parent = tree;
    tree->parent = grandparent;
}

void _rb_rotate(rb_tree_t* tree) {
    if (bin_tree_is_left(tree)) {
        _rb_rotate_right(tree);
    } else {
        _rb_rotate_left(tree);
    }
}

rb_color_t _rb_color(rb_tree_t* tree) {
    return ((rb_vertex_data_t*)tree->data)->color;
}

void _rb_set_color(rb_tree_t* tree, rb_color_t color) {
    ((rb_vertex_data_t*)tree->data)->color = color;
}

void _rb_show(binary_tree_t* tree, int indent, char pos) {
    for (int i = 0; i < indent; i++) {
        printf(" ");
    }

    if (_rb_color(tree) == RED) {
        printf("\033[0;31m●\033[0m ");
    } else {
        printf("◯ ");
    }
    if (tree->parent == NULL) {
        printf("P=nil ");
    } else {
        printf("P=%d ", tree->parent->weight);
    }

    printf("%c {%d}\n", pos, tree->weight);

    if (tree->left != NULL) {
        _rb_show(tree->left, indent + 4, 'L');
    }
    if (tree->right != NULL) {
        _rb_show(tree->right, indent + 4, 'R');
    }
}
