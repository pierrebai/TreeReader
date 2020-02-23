
add_library(TreeReaderTests SHARED
   SimplerTreeReaderTests.cpp
   TextTreeTests.cpp
   TreeFilterMakerTests.cpp
   TreeFilterTests.cpp
   TreeReaderHelpersTests.cpp
   TreeReaderTestHelpers.cpp)

target_link_libraries(TreeReaderTests PUBLIC TreeReader)

target_compile_features(TreeReaderTests PUBLIC cxx_std_20)

target_include_directories(TreeReaderTests PUBLIC
   "${PROJECT_SOURCE_DIR}/TreeReader")

