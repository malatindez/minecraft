set(MINECRAFT_SOURCES ${SOURCES})
list(FILTER MINECRAFT_SOURCES INCLUDE REGEX "${SRC_DIR}/minecraft/*")

add_executable(minecraft ${INCLUDES} ${MINECRAFT_SOURCES})
target_include_directories(minecraft PRIVATE "${INC_DIR}")
target_include_directories(minecraft PRIVATE "${SRC_DIR}/minecraft")
target_add_libraries(minecraft "glfw" "glad" "glm" "freetype" "stb")
add_custom_target(
  pack_resources
  COMMAND
    "${CMAKE_COMMAND}" -E copy_if_different
    "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>/packer.exe"
    "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/packer.exe"
  COMMAND
    "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/packer.exe"
    "${CMAKE_BINARY_DIR}/runtime_directory/resource.pack"
    "${CMAKE_CURRENT_SOURCE_DIR}/resources"
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  COMMENT
    "Packing the resources folder to resource.pack @ ${CMAKE_BINARY_DIR}/runtime_directory/resource.pack"
  SOURCES ${PACKER_SOURCES})
add_dependencies(pack_resources packer)
add_dependencies(minecraft pack_resources)
