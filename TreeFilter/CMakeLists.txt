add_executable(TreeFilter TreeFilter.cpp)

target_link_libraries(TreeFilter PUBLIC TreeReader)

target_compile_features(TreeFilter PUBLIC cxx_std_20)

target_include_directories(TreeFilter PUBLIC
   "${PROJECT_SOURCE_DIR}/TreeReader")

