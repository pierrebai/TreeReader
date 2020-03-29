cmake_minimum_required(VERSION 3.7.0)

project(TreeReaderProject)

# The TreeReader library, its tests and the TreeFilter command-line program

add_definitions(-DUNICODE)
add_definitions(-D_UNICODE)

add_subdirectory(tree_reader)
add_subdirectory(tree_reader_tests)
add_subdirectory(QtAdditions)
add_subdirectory(tree_filter)
add_subdirectory(TreeFilterApp)

