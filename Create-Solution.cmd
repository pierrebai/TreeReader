@echo off
setlocal
mkdir Build
cd Build
rem set QT5_DIR to where the Qt5 visual-studio 2019 64-bit files are installed,
rem for example C:\Outils\Qt\5.15.0\msvc2019_64
set CMAKE_PREFIX_PATH=%QT5_DIR%
cmake .. -G "Visual Studio 16 2019" -DCMAKE_GENERATOR_TOOLSET=v142
