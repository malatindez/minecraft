set(MINECRAFT_SOURCES ${SOURCES})
list(FILTER MINECRAFT_SOURCES INCLUDE REGEX "${SRC_DIR}/minecraft/*")

add_executable(minecraft ${INCLUDES} ${MINECRAFT_SOURCES})
if(MSVC)
  target_compile_options(minecraft PRIVATE /W4 /WX)
else()
  target_compile_options(minecraft  PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

target_include_directories(minecraft PRIVATE "${INC_DIR}")
target_include_directories(minecraft PRIVATE "${SRC_DIR}/minecraft")
target_add_libraries(minecraft "glfw" "glad" "glm" "freetype" "stb" "spdlog")
IF (WIN32)
add_custom_target(
  pack_resources
  COMMAND
    "${CMAKE_COMMAND}" -E copy_if_different
    "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>/packer.exe"
    "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/packer.exe"
  COMMAND
    "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/packer.exe"
    "${CMAKE_BINARY_DIR}/runtime_directory/resources.pack"
    "${CMAKE_CURRENT_SOURCE_DIR}/resources"
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  COMMENT
    "Packing the resources folder to resources.pack @ ${CMAKE_BINARY_DIR}/runtime_directory/resource.pack"
  SOURCES ${PACKER_SOURCES})
ELSE()
add_custom_target(
  pack_resources
  COMMAND
    "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/packer"
    "${CMAKE_BINARY_DIR}/runtime_directory/resources.pack"
    "${CMAKE_CURRENT_SOURCE_DIR}/resources"
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  COMMENT
    "Packing the resources folder to resources.pack @ ${CMAKE_BINARY_DIR}/runtime_directory/resource.pack"
  SOURCES ${PACKER_SOURCES})
ENDIF()
add_dependencies(pack_resources packer)
add_dependencies(minecraft pack_resources)
