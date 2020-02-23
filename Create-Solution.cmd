@echo off
setlocal
mkdir Build
cd Build
cmake .. -G "Visual Studio 16 2019" -DCMAKE_GENERATOR_TOOLSET=v141
