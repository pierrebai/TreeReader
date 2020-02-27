
add_library(TreeReader
   BuffersTextHolder.cpp
   SimpleTreeReader.cpp
   SimpleTreeWriter.cpp
   TextTree.cpp
   TextTreeVisitor.cpp
   TreeFilter.cpp
   TreeFilterMaker.cpp
   SimpleTreeFilterMaker.cpp
   TreeFilterCommands.cpp
   TreeReaderHelpers.cpp
   NamedFilters.cpp

   BuffersTextHolder.h
   SimpleTreeReader.h
   SimpleTreeWriter.h
   TextLinesTextHolder.h
   TextTree.h
   TextTreeVisitor.h
   TreeFilter.h
   TreeFilterMaker.h
   TreeFilterCommands.h
   TreeReader.h
   TreeReaderHelpers.h
   NamedFilters.h
)

target_compile_features(TreeReader PUBLIC cxx_std_20)
