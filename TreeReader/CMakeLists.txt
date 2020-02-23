
add_library(TreeReader
   BuffersTextHolder.cpp
   SimpleTreeReader.cpp
   TextTree.cpp
   TextTreeVisitor.cpp
   TreeFilter.cpp
   TreeFilterMaker.cpp
   TreeReaderHelpers.cpp)

target_compile_features(TreeReader PUBLIC cxx_std_20)
