#include "binary_tree.h"
#include <stdlib.h>

binary_tree_t* bin_tree_make(int root_label, int root_value) {
    binary_tree_t* vertex = malloc(sizeof(binary_tree_t));
    vertex->label = root_label;
    vertex->weight = root_value;
    vertex->left = vertex->right = vertex->parent = NULL;
    vertex->data = NULL;
    return vertex;
}

void bin_tree_free(binary_tree_t* tree) {
    if (tree == NULL) {
        return;
    }

    binary_tree_t* curr = tree;
    bool stop = false;
    while (!stop) {
        if (curr->left != NULL) {
            curr = curr->left;
        } else if (curr->right != NULL) {
            curr = curr->right;
        } else {
            stop = curr == tree;
            binary_tree_t* to_delete = curr;
            curr = to_delete->parent;
            if (curr != NULL) {
                if (bin_tree_is_left(to_delete)) {
                    curr->left = NULL;
                } else {
                    curr->right = NULL;
                }
            }
            if (to_delete->data != NULL) {
                free(to_delete->data);
            }
            free(to_delete);
        }
    }
}

bool bin_tree_is_left(binary_tree_t* node) {
    if (node != NULL && node->parent != NULL && node->parent->left == node) {
        return true;
    }
    return false;
}

bool bin_tree_is_right(binary_tree_t* node) {
    if (node != NULL && node->parent != NULL && node->parent->right == node) {
        return true;
    }
    return false;
}

binary_tree_t* bin_tree_find(binary_tree_t* tree, int value) {
    binary_tree_t* root = tree;
    while (root != NULL) {
        if (root->weight == value) {
            return root;
        } else if (root->weight > value) {
            root = root->left;
        } else {
            root = root->right;
        }
    }
    return NULL;
}

binary_tree_t* bin_tree_insert(binary_tree_t* tree, int label, int value) {
    binary_tree_t* node = bin_tree_make(label, value);
    binary_tree_t* curr = tree;
    while (true) {
        if (curr->weight >= node->weight) {
            if (curr->left == NULL) {
                curr->left = node;
                node->parent = curr;
                break;
            } else {
                curr = curr->left;
            }
        } else {
            if (curr->right == NULL) {
                curr->right = node;
                node->parent = curr;
                break;
            } else {
                curr = curr->right;
            }
        }
    }
    return node;
}
