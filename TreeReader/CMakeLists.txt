
add_library(TreeReader
   TreeReader.h

   BuffersTextHolder.cpp      BuffersTextHolder.h TextLinesTextHolder.h
   SimpleTreeReader.cpp       SimpleTreeReader.h
   SimpleTreeWriter.cpp       SimpleTreeWriter.h
   TextTree.cpp               TextTree.h
   TextTreeVisitor.cpp        TextTreeVisitor.h
   TreeFilter.cpp             TreeFilter.h
   TreeFilterHelpers.cpp      TreeFilterHelpers.h
   TreeFilterMaker.cpp        TreeFilterMaker.h
   SimpleTreeFilterMaker.cpp
   TreeFilterCommands.cpp     TreeFilterCommands.h
   TreeReaderHelpers.cpp      TreeReaderHelpers.h
   NamedFilters.cpp           NamedFilters.h
   UndoStack.cpp              UndoStack.h
)

target_compile_features(TreeReader PUBLIC cxx_std_20)

