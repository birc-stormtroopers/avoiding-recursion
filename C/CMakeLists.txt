cmake_minimum_required(VERSION 3.21)
project(tree-traversal)
enable_testing()

add_library(traversal dynarr.c tree.c)

add_executable(dynarr_test dynarr_test.c)
target_link_libraries(dynarr_test traversal)
add_test(dynarr_test dynarr_test)

add_executable(traversal_test traversal_test.c)
target_link_libraries(traversal_test traversal)
add_test(traversal_test traversal_test)
