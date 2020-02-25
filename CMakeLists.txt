cmake_minimum_required(VERSION 3.7.0)

project(TreeReaderProject)

# The TreeReader library, its tests and the TreeFilter command-line program

add_subdirectory(TreeReader)
add_subdirectory(TreeReaderTests)
add_subdirectory(TreeFilter)
add_subdirectory(TreeFilterApp)

