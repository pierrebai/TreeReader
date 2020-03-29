# Tree Filter App
A Windows application to ready, display and filter tree-like data.

# Description
The application can read very large text file representing a tree-like structure. The structrure of the tree is derived from the indentation of each line of the text file.

You can create tree filters by drag-and-drop of filter elements. Each filter element performs a simple task, like matching text or keeping a node in the tree only if a child node meets some sub-filter criteria.

There is also a quick interactive text search filtering.

![User Interface](https://github.com/pierrebai/TreeReader/blob/master/TreeFilterApp.png "User Interface")

# Dependencies and Build
The project requires Qt. It was built using Qt 5.12. It uses CMake to build the project. CMake 3.16.4 was used.

A script to generate a Visual-Studio solution is provided. In order for CMake to find Qt, the environment variable QT5_DIR must be defined and must point to the Qt 5.12 directory. For example:

    QT5_DIR=C:\Outils\Qt\5.12.1\

Alternatively, to invoke cmake directly, it needs to be told where to find Qt. It needs the environment variable CMAKE_PREFIX_PATH set to the location of Qt. For example:

    CMAKE_PREFIX_PATH=%QT5_DIR%\msvc2017_64

The code was written and tested with Visual Studio 2017, community edition.
