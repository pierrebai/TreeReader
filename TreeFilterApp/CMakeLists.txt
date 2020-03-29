# Qt 5 stuff

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Core REQUIRED)
find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt5 COMPONENTS Gui REQUIRED)
find_package(Qt5 COMPONENTS WinExtras REQUIRED)
find_package(Qt5 COMPONENTS LinguistTools REQUIRED)

qt5_create_translation(qmFiles
   ${CMAKE_SOURCE_DIR}
   TreeFilterApp_fr.ts
)

add_executable(TreeFilterApp WIN32
   TreeFilterApp.cpp
   FilterEditor.cpp           FilterEditor.h
   MainWindow.cpp             MainWindow.h
   TextTreeModel.cpp          TextTreeModel.h
   TextTreeSubWindow.cpp      TextTreeSubWindow.h
   TreeFilterListItem.cpp     TreeFilterListItem.h
   TreeFilterListWidget.cpp   TreeFilterListWidget.h
   OptionsDialog.cpp          OptionsDialog.h

   Resource.rc             resource.h

   ToDo.txt

   ${qmFiles}

   #resources.qrc
)

target_link_libraries(TreeFilterApp PUBLIC
   tree_reader
   QtAdditions
   Qt5::Widgets Qt5::Gui Qt5::Core Qt5::WinExtras
)

target_compile_features(TreeFilterApp PUBLIC cxx_std_20)

target_include_directories(TreeFilterApp PUBLIC
   "${PROJECT_SOURCE_DIR}/tree_reader/inc"
   "${PROJECT_SOURCE_DIR}/QtAdditions/inc"
)

add_custom_command(TARGET TreeFilterApp POST_BUILD
   COMMAND "${Qt5_DIR}/../../../bin/windeployqt.exe" --no-webkit2 --no-angle --no-system-d3d-compiler --no-opengl-sw "$<TARGET_FILE:TreeFilterApp>"
   COMMAND ${CMAKE_COMMAND} -E copy "${qmFiles}" "$<TARGET_FILE_DIR:TreeFilterApp>"
)

install(
   DIRECTORY
   "$<TARGET_FILE_DIR:TreeFilterApp>/"
   DESTINATION .
   COMPONENT application
   PATTERN "*.pdb" EXCLUDE 
   PATTERN "*.ilk" EXCLUDE 
)

set(CPACK_GENERATOR NSIS)
set(CPACK_PACKAGE_NAME "TreeFilterApp")
set(CPACK_PACKAGE_VENDOR "Pierre Baillargeon")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Tree Filter App - An application to filter trees of text.")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "Tree Filter App")
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE")

set(CPACK_NSIS_MUI_ICON "${PROJECT_SOURCE_DIR}/TreeFilterApp/icon.ico")
set(CPACK_NSIS_MUI_UNIICON "${PROJECT_SOURCE_DIR}/TreeFilterApp/icon.ico")
set(CPACK_NSIS_INSTALLED_ICON_NAME "TreeFilterApp.exe")
set(CPACK_NSIS_DISPLAY_NAME "Tree Filter App")

INCLUDE(CPack)

