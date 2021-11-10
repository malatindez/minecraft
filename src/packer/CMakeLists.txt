set(PACKER_SOURCES ${SOURCES})
list(FILTER PACKER_SOURCES INCLUDE REGEX "${SRC_DIR}/packer/*")
set(INC_SOURCES ${SOURCES})
list(FILTER INC_SOURCES INCLUDE REGEX "${SRC_DIR}/inc/*")
add_executable(packer ${PACKER_SOURCES} ${INC_SOURCES})
target_include_directories(packer PRIVATE "${INC_DIR}")
target_include_directories(packer PRIVATE "${SRC_DIR}/packer")

target_add_libraries(packer "glfw" "glad" "glm" "freetype" "stb")
