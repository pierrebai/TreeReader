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
   TreeReader
   QtAdditions
   Qt5::Widgets Qt5::Gui Qt5::Core Qt5::WinExtras
)

target_compile_features(TreeFilterApp PUBLIC cxx_std_20)

target_include_directories(TreeFilterApp PUBLIC
   "${PROJECT_SOURCE_DIR}/TreeReader"
   "${PROJECT_SOURCE_DIR}/QtAdditions"
)

add_custom_command(TARGET TreeFilterApp POST_BUILD
   COMMAND "${Qt5_DIR}/../../../bin/windeployqt.exe" --no-webkit2 --no-angle --no-system-d3d-compiler --no-opengl-sw "$<TARGET_FILE:TreeFilterApp>"
   COMMAND ${CMAKE_COMMAND} -E copy "${qmFiles}" "$<TARGET_FILE_DIR:TreeFilterApp>"
)

install(TARGETS TreeFilterApp
   DESTINATION programs
   COMPONENT application
)

get_target_property(Qt5CoreDll Qt5::Core LOCATION)
get_target_property(Qt5WidgetsDll Qt5::Widgets LOCATION)
get_target_property(Qt5GuiDll Qt5::Gui LOCATION)
get_target_property(Qt5WinExtrasDll Qt5::WinExtras LOCATION)
get_filename_component(Qt5DllDir "${Qt5CoreDll}" DIRECTORY)

install(
   FILES
   "${Qt5DllDir}/../plugins/iconengines/qsvgicon.dll"
   DESTINATION programs
   COMPONENT application
)

install(
   FILES
   "${Qt5DllDir}/../plugins/imageformats/qgif.dll"
   "${Qt5DllDir}/../plugins/imageformats/qicns.dll"
   "${Qt5DllDir}/../plugins/imageformats/qico.dll"
   "${Qt5DllDir}/../plugins/imageformats/qjpeg.dll"
   "${Qt5DllDir}/../plugins/imageformats/qsvg.dll"
   "${Qt5DllDir}/../plugins/imageformats/qtga.dll"
   "${Qt5DllDir}/../plugins/imageformats/qtiff.dll"
   "${Qt5DllDir}/../plugins/imageformats/qwbmp.dll"
   "${Qt5DllDir}/../plugins/imageformats/qwebp.dll"
   DESTINATION programs
   COMPONENT application
)

install(
   FILES
   "${Qt5DllDir}/../plugins/platforms/qwindows.dll"
   DESTINATION programs
   COMPONENT application
)

install(
   FILES
   "${Qt5DllDir}/../plugins/styles/qwindowsvistastyle.dll"
   DESTINATION programs
   COMPONENT application
)

install(
   FILES
   "${Qt5DllDir}/../translations/qt_en.qm"
   "${Qt5DllDir}/../translations/qt_fr.qm"
   DESTINATION programs
   COMPONENT application
)

install(
   FILES
   "${qmFiles}"
   "${Qt5CoreDll}"
   "${Qt5WidgetsDll}"
   "${Qt5GuiDll}"
   "${Qt5WinExtrasDll}"
   DESTINATION programs
   COMPONENT application
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
SET(CPACK_NSIS_MODIFY_PATH ON)

INCLUDE(CPack)
