add_executable(tree_filter tree_filter.cpp)

target_link_libraries(tree_filter PUBLIC
   tree_reader
   dak_utility
)

target_compile_features(tree_filter PUBLIC cxx_std_20)

