cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(minecraft LANGUAGES CXX)

set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(INC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/inc")
set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs")

include("${LIB_DIR}/CMakeLists.txt")

file(GLOB_RECURSE SOURCES ${SRC_DIR} *.*)
list(FILTER SOURCES INCLUDE REGEX "${SRC_DIR}/*")

foreach(_source IN ITEMS ${SOURCES})
  if(IS_ABSOLUTE "${_source}")
    file(RELATIVE_PATH _source_rel "${CMAKE_CURRENT_SOURCE_DIR}" "${_source}")
  else()
    set(_source_rel "${_source}")
  endif()
  get_filename_component(_source_path "${_source_rel}" PATH)
  string(REPLACE "/" "\\" _source_path_msvc "${_source_path}")
  source_group("${_source_path_msvc}" FILES "${_source}")
endforeach()

set(INCLUDES ${SOURCES})
list(FILTER INCLUDES INCLUDE REGEX "${SRC_DIR}/inc/*")

make_directory(${CMAKE_BINARY_DIR}/runtime_directory)
make_directory(${CMAKE_BINARY_DIR}/runtime_directory/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/runtime_directory/bin")
include("${SRC_DIR}/packer/CMakeLists.txt")
include("${SRC_DIR}/minecraft/CMakeLists.txt")
include("${SRC_DIR}/tests/CMakeLists.txt")

include_directories("${INC_DIR}")
