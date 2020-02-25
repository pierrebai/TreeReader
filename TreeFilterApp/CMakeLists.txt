# Qt 5 stuff

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Core REQUIRED)
find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt5 COMPONENTS Gui REQUIRED)

add_executable(TreeFilterApp WIN32
   TreeFilterApp.cpp
   #mainwindow.ui
   #resources.qrc
)

target_link_libraries(TreeFilterApp PUBLIC TreeReader Qt5::Widgets Qt5::Gui Qt5::Core)

target_compile_features(TreeFilterApp PUBLIC cxx_std_20)

target_include_directories(TreeFilterApp PUBLIC
   "${PROJECT_SOURCE_DIR}/TreeReader"
)

