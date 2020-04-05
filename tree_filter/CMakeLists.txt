add_executable(tree_filter tree_filter.cpp)

target_link_libraries(tree_filter PUBLIC
   tree_reader
   undo_redo
)

target_compile_features(tree_filter PUBLIC cxx_std_20)

target_include_directories(tree_filter PUBLIC
   "${PROJECT_SOURCE_DIR}/tree_reader/inc"
   "${PROJECT_SOURCE_DIR}/SimpleUndoRedo/undo_redo/inc"
)

