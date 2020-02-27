cmake_minimum_required(VERSION 3.7.0)

project(TreeReaderProject)

# The TreeReader library, its tests and the TreeFilter command-line program

add_definitions(-DUNICODE)
add_definitions(-D_UNICODE)

add_subdirectory(TreeReader)
add_subdirectory(TreeReaderTests)
add_subdirectory(TreeFilter)
add_subdirectory(TreeFilterApp)

