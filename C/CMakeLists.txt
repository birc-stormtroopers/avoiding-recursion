cmake_minimum_required(VERSION 3.21)
project(tree-traversal)
enable_testing()

add_library(traversal dynarr.c tree.c stack.c stack_traversal.c)

add_executable(dynarr_test dynarr_test.c)
target_link_libraries(dynarr_test traversal)
add_test(dynarr_test dynarr_test)

add_executable(stack_test stack_test.c)
target_link_libraries(stack_test traversal)
add_test(stack_test stack_test)

add_executable(fib fib.c)
add_test(fib fib)

add_executable(traversal_test traversal_test.c)
target_link_libraries(traversal_test traversal)
add_test(traversal_test traversal_test)
