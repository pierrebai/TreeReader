cmake_minimum_required(VERSION 3.7.0)

project(TreeReaderProject)

# The TreeReader library, its tests and the TreeFilter command-line program

add_subdirectory(TreeReader)
add_subdirectory(TreeReaderTests)
add_subdirectory(TreeFilter)

# Qt 5 stuff
# set(CMAKE_AUTOMOC ON)
# set(CMAKE_AUTORCC ON)
# set(CMAKE_AUTOUIC ON)
# 
# find_package(Qt5 COMPONENTS Core REQUIRED)
# find_package(Qt5 COMPONENTS Widgets REQUIRED)
# find_package(Qt5 COMPONENTS Gui REQUIRED)
# 
# # TreeReader library
# 
# 
# add_executable(TreeFilterApp
#     mainwindow.ui
#     mainwindow.cpp
#     main.cpp
#     resources.qrc
# )
# 
# target_link_libraries(helloworld Qt5::Widgets Qt5:Gui Qt5:Core)

