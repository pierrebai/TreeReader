# Qt 5 stuff

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Core REQUIRED)
find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt5 COMPONENTS Gui REQUIRED)
find_package(Qt5 COMPONENTS WinExtras REQUIRED)

add_executable(TreeFilterApp WIN32
   TreeFilterApp.cpp
   FilterEditor.cpp        FilterEditor.h
   MainWindow.cpp          MainWindow.h
   QtUtilities.cpp         QtUtilities.h
   TextTreeModel.cpp       TextTreeModel.h
   TreeFilterPanel.cpp     TreeFilterPanel.h

   Resource.rc             resource.h

   ToDo.txt

   #resources.qrc
)

target_link_libraries(TreeFilterApp PUBLIC TreeReader Qt5::Widgets Qt5::Gui Qt5::Core Qt5::WinExtras)

target_compile_features(TreeFilterApp PUBLIC cxx_std_20)

target_include_directories(TreeFilterApp PUBLIC
   "${PROJECT_SOURCE_DIR}/TreeReader"
)

add_custom_command(TARGET TreeFilterApp POST_BUILD
   COMMAND "${Qt5_DIR}/../../../bin/windeployqt.exe" --no-webkit2 --no-angle --no-system-d3d-compiler --no-opengl-sw "$<TARGET_FILE:TreeFilterApp>"
)

