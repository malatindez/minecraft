git submodule update --init
mkdir build
cd build
cmake -D CMAKE_C_COMPILER=gcc-10 -D CMAKE_CXX_COMPILER=g++-10 .. .
make
runtime_directory/bin/runUnitTests