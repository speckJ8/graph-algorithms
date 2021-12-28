cmake_minimum_required(VERSION 3.10)

project(GraphAlgorithms)
add_compile_options(-Wall -g -O0)
set(CMAKE_C_STANDARD 11)

add_library(BinaryTree binary_tree.c)
add_library(RBTree rb_tree.c)
add_library(Gabow gabow.c)
# add_library(Boruvka boruvka.c)

target_link_libraries(RBTree BinaryTree)

enable_testing()

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(RBTreeTest rb_tree_test.cc)
target_link_libraries(RBTreeTest RBTree gtest_main)

include(GoogleTest)
gtest_discover_tests(RBTreeTest)
