
add_library(tree_reader
   inc/dak/tree_reader/tree_reader.h

   src/buffers_text_holder.cpp       inc/dak/tree_reader/buffers_text_holder.h inc/dak/tree_reader/text_lines_text_holder.h
   src/simple_tree_reader.cpp        inc/dak/tree_reader/simple_tree_reader.h
   src/simple_tree_writer.cpp        inc/dak/tree_reader/simple_tree_writer.h
   src/text_tree.cpp                 inc/dak/tree_reader/text_tree.h
   src/text_tree_visitor.cpp         inc/dak/tree_reader/text_tree_visitor.h
   src/tree_filter.cpp               inc/dak/tree_reader/tree_filter.h
   src/tree_filtering.cpp            inc/dak/tree_reader/tree_filtering.h
   src/tree_filter_helpers.cpp       inc/dak/tree_reader/tree_filter_helpers.h
   src/tree_filter_maker.cpp         inc/dak/tree_reader/tree_filter_maker.h
   src/simple_tree_filter_maker.cpp
   src/global_commands.cpp           inc/dak/tree_reader/global_commands.h
   src/tree_commands.cpp             inc/dak/tree_reader/tree_commands.h
   src/tree_filter_command_line.cpp  inc/dak/tree_reader/tree_filter_command_line.h
   src/named_filters.cpp             inc/dak/tree_reader/named_filters.h

   src/text.cpp                      inc/dak/utility/text.h
                                     inc/dak/utility/exceptions.h 
)

target_include_directories(tree_reader PUBLIC
   inc
   "${PROJECT_SOURCE_DIR}/dak_utility/utility/include"
)

target_compile_features(tree_reader PUBLIC cxx_std_20)

