
add_library(tree_reader_tests SHARED
   simple_tree_reader_tests.cpp
   named_filters_tests.cpp
   text_tree_tests.cpp
   text_tree_visitor_tests.cpp
   tree_filter_maker_tests.cpp
   tree_filter_tests.cpp
   text_tests.cpp
   tree_reader_test_helpers.cpp
   undo_stack_tests.cpp
   tree_filter_commands_tests.cpp

   tree_reader_test_helpers.h
)

target_link_libraries(tree_reader_tests PUBLIC
   tree_reader
   utility
)

target_compile_features(tree_reader_tests PUBLIC cxx_std_20)

target_include_directories(tree_reader_tests PUBLIC
   "${PROJECT_SOURCE_DIR}/tree_reader/inc"
   "${PROJECT_SOURCE_DIR}/dak/utility/include"
)

