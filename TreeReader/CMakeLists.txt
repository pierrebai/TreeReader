
add_library(TreeReader
   BuffersTextHolder.cpp
   SimpleTreeReader.cpp
   TextTree.cpp
   TextTreeVisitor.cpp
   TreeFilter.cpp
   TreeFilterMaker.cpp
   TreeReaderHelpers.cpp

   BuffersTextHolder.h
   SimpleTreeReader.h
   TextLinesTextHolder.h
   TextTree.h
   TextTreeVisitor.h
   TreeFilter.h
   TreeFilterMaker.h
   TreeReader.h
   TreeReaderHelpers.h
)

target_compile_features(TreeReader PUBLIC cxx_std_20)
