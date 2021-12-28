#include <gtest/gtest.h>
#include <stdio.h>

extern "C" {
#include "rb_tree.h"
}

TEST(rb_tree, basic) {
    rb_tree_t* tree = rb_make(0, 11);

    tree = rb_insert(tree, 1, 10);
    ASSERT_EQ(tree->weight, 11);
    ASSERT_EQ(tree->left->weight, 10);

    tree = rb_insert(tree, 2, 4);
    ASSERT_EQ(tree->weight, 10);
    ASSERT_EQ(tree->right->weight, 11);
    ASSERT_EQ(tree->left->weight, 4);

    tree = rb_insert(tree, 3, 16);
    ASSERT_EQ(tree->weight, 10);
    ASSERT_EQ(tree->right->weight, 11);
    ASSERT_EQ(tree->left->weight, 4);
    ASSERT_EQ(tree->right->right->weight, 16);

    tree = rb_insert(tree, 5, 13);
    ASSERT_EQ(tree->weight, 10);
    ASSERT_EQ(tree->right->weight, 13);
    ASSERT_EQ(tree->left->weight, 4);
    ASSERT_EQ(tree->right->right->weight, 16);
    ASSERT_EQ(tree->right->left->weight, 11);

    tree = rb_insert(tree, 6, 7);
    ASSERT_EQ(tree->weight, 10);
    ASSERT_EQ(tree->right->weight, 13);
    ASSERT_EQ(tree->left->weight, 4);
    ASSERT_EQ(tree->right->right->weight, 16);
    ASSERT_EQ(tree->right->left->weight, 11);
    ASSERT_EQ(tree->left->right->weight, 7);

    tree = rb_insert(tree, 4, 1);
    ASSERT_EQ(tree->weight, 10);
    ASSERT_EQ(tree->right->weight, 13);
    ASSERT_EQ(tree->left->weight, 4);
    ASSERT_EQ(tree->right->right->weight, 16);
    ASSERT_EQ(tree->right->left->weight, 11);
    ASSERT_EQ(tree->left->left->weight, 1);
    ASSERT_EQ(tree->left->right->weight, 7);

    tree = rb_insert(tree, 7, 15);
    ASSERT_EQ(tree->weight, 10);
    ASSERT_EQ(tree->right->weight, 13);
    ASSERT_EQ(tree->left->weight, 4);
    ASSERT_EQ(tree->right->right->weight, 16);
    ASSERT_EQ(tree->right->left->weight, 11);
    ASSERT_EQ(tree->left->left->weight, 1);
    ASSERT_EQ(tree->left->right->weight, 7);
    ASSERT_EQ(tree->right->right->left->weight, 15);

    tree = rb_insert(tree, 8, 3);
    ASSERT_EQ(tree->weight, 10);
    ASSERT_EQ(tree->right->weight, 13);
    ASSERT_EQ(tree->left->weight, 3);
    ASSERT_EQ(tree->right->right->weight, 16);
    ASSERT_EQ(tree->right->left->weight, 11);
    ASSERT_EQ(tree->left->left->weight, 1);
    ASSERT_EQ(tree->left->right->weight, 4);
    ASSERT_EQ(tree->right->right->left->weight, 15);
    ASSERT_EQ(tree->left->right->right->weight, 7);

    tree = rb_insert(tree, 9, 15);
    ASSERT_EQ(tree->weight, 13);
    ASSERT_EQ(tree->left->weight, 10);
    ASSERT_EQ(tree->right->weight, 15);
    ASSERT_EQ(tree->left->left->weight, 3);
    ASSERT_EQ(tree->left->right->weight, 11);
    ASSERT_EQ(tree->right->left->weight, 15);
    ASSERT_EQ(tree->right->right->weight, 16);
    ASSERT_EQ(tree->left->left->left->weight, 1);
    ASSERT_EQ(tree->left->left->right->weight, 4);
    ASSERT_EQ(tree->left->left->right->right->weight, 7);

    rb_show(tree);
    rb_free(tree);
}
