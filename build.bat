git submodule update --init
if not exist build mkdir build
cd build
cmake .. .
cmake --build . --config Release
.\Release\runUnitTests.exe
explorer .
pause