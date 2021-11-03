set(MINECRAFT_SOURCES ${SOURCES})
list(FILTER MINECRAFT_SOURCES INCLUDE REGEX "${SRC_DIR}/minecraft/*")

add_executable(minecraft ${INCLUDES} ${MINECRAFT_SOURCES})
target_include_directories(minecraft PRIVATE "${INC_DIR}")
target_include_directories(minecraft PRIVATE "${SRC_DIR}/minecraft")
target_add_libraries(minecraft "glfw" "glad" "glm" "freetype" "stb")
