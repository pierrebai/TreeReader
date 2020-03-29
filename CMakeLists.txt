cmake_minimum_required(VERSION 3.7.0)

project(TreeReaderProject)

# The tree_reader library, its tests and the tree_filter command-line program

add_definitions(-DUNICODE)
add_definitions(-D_UNICODE)

add_subdirectory(tree_reader)
add_subdirectory(tree_reader_tests)
<<<<<<< Updated upstream
add_subdirectory(QtAdditions)
=======
add_subdirectory(Qtadditions)
>>>>>>> Stashed changes
add_subdirectory(tree_filter)
add_subdirectory(TreeFilterApp)

