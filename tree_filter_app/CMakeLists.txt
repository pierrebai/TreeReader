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
   tree_filter_app_fr.ts
)

add_executable(tree_filter_app WIN32
   tree_filter_app.cpp
   filter_editor.cpp             filter_editor.h
   main_window.cpp               main_window.h
   text_tree_model.cpp           text_tree_model.h
   text_tree_sub_window.cpp      text_tree_sub_window.h
   tree_filter_list_item.cpp     tree_filter_list_item.h
   tree_filter_list_widget.cpp   tree_filter_list_widget.h
   options_dialog.cpp            options_dialog.h

   resource.rc                   resource.h

   todo.txt

   ${qmFiles}

   #resources.qrc
)

target_link_libraries(tree_filter_app PUBLIC
   tree_reader
   dak_utility
   QtAdditions
   Qt5::Widgets Qt5::Gui Qt5::Core Qt5::WinExtras
)

target_compile_features(tree_filter_app PUBLIC cxx_std_20)

add_custom_command(TARGET tree_filter_app POST_BUILD
   COMMAND "${Qt5_DIR}/../../../bin/windeployqt.exe" --no-webkit2 --no-angle --no-system-d3d-compiler --no-opengl-sw "$<TARGET_FILE:tree_filter_app>"
   COMMAND ${CMAKE_COMMAND} -E copy "${qmFiles}" "$<TARGET_FILE_DIR:tree_filter_app>"
)

install(
   DIRECTORY
   "$<TARGET_FILE_DIR:tree_filter_app>/"
   DESTINATION .
   COMPONENT application
   PATTERN "*.pdb" EXCLUDE 
   PATTERN "*.ilk" EXCLUDE 
)

set(CPACK_GENERATOR NSIS)
set(CPACK_PACKAGE_NAME "TreeFilterApp")
set(CPACK_PACKAGE_VENDOR "Pierre Baillargeon")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Tree filter App - An application to filter trees of text.")
set(CPACK_PACKAGE_VERSION "1.0.1")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "1")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "Tree Filter")
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE")

set(CPACK_NSIS_MUI_ICON "${PROJECT_SOURCE_DIR}/tree_filter_app/icon.ico")
set(CPACK_NSIS_MUI_UNIICON "${PROJECT_SOURCE_DIR}/tree_filter_app/icon.ico")
set(CPACK_NSIS_INSTALLED_ICON_NAME "tree_filter_app.exe")
set(CPACK_NSIS_DISPLAY_NAME "Tree Filter")

set(CPACK_NSIS_CREATE_ICONS_EXTRA
    "CreateShortCut '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\Tree Filter.lnk' '$INSTDIR\\\\tree_filter_app.exe'"
)
set(CPACK_NSIS_DELETE_ICONS_EXTRA
    "Delete '$SMPROGRAMS\\\\$START_MENU\\\\Tree Filter.lnk'"
)

INCLUDE(CPack)

