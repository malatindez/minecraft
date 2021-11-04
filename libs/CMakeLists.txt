# load libraries glfw
set(GLFW_DIR "${LIB_DIR}/glfw")
set(GLFW_BUILD_EXAMPLES
    OFF
    CACHE INTERNAL "Build the GLFW example programs")
set(GLFW_BUILD_TESTS
    OFF
    CACHE INTERNAL "Build the GLFW test programs")
set(GLFW_BUILD_DOCS
    OFF
    CACHE INTERNAL "Build the GLFW documentation")
set(GLFW_INSTALL
    OFF
    CACHE INTERNAL "Generate installation target")
add_subdirectory("${GLFW_DIR}" EXCLUDE_FROM_ALL)
# glad
set(GLAD_DIR "${LIB_DIR}/glad")
add_library("glad" STATIC EXCLUDE_FROM_ALL "${GLAD_DIR}/src/glad.c")
target_include_directories("glad" PRIVATE "${GLAD_DIR}/include")
# glm
set(GLM_DIR "${LIB_DIR}/glm")
add_subdirectory("${GLM_DIR}" EXCLUDE_FROM_ALL)
# stb
set(STB_DIR "${LIB_DIR}/stb")
# freetype
set(FREETYPE_DIR "${LIB_DIR}/freetype")
set(FT_DISABLE_ZLIB ON)
set(FT_DISABLE_BZIP2 ON)
set(FT_DISABLE_PNG ON)
set(FT_DISABLE_HARFBUZZ ON)
set(FT_DISABLE_BROTLI ON)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
add_subdirectory("${FREETYPE_DIR}" EXCLUDE_FROM_ALL)
# google test
set(GTEST_DIR "${LIB_DIR}/googletest")
set(gtest_force_shared_crt
    ON
    CACHE BOOL "" FORCE)

add_subdirectory(${GTEST_DIR} EXCLUDE_FROM_ALL)

function(target_add_glfw TARGET)
  target_include_directories("${TARGET}" PRIVATE "${GLFW_DIR}/include")
  target_compile_definitions("${TARGET}" PRIVATE "GLFW_INCLUDE_NONE")
  target_link_libraries("${TARGET}" "glfw" "${GLFW_LIBRARIES}")
endfunction(target_add_glfw)

function(target_add_glad TARGET)
  target_include_directories("${TARGET}" PRIVATE "${GLAD_DIR}/include")
  target_link_libraries("${TARGET}" glad)
endfunction(target_add_glad)

function(target_add_stb TARGET)
  target_include_directories("${TARGET}" PRIVATE "${STB_DIR}/include")
endfunction(target_add_stb)

function(target_add_glm TARGET)
  target_include_directories("${TARGET}" PRIVATE "${GLM_DIR}")
endfunction(target_add_glm)

function(target_add_freetype TARGET)
  target_include_directories("${TARGET}" PRIVATE "${FREETYPE_DIR}/include")
endfunction(target_add_freetype)

function(target_add_gtest TARGET)
  enable_testing()
  include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
  include_directories(${INCLUDES})
endfunction(target_add_gtest)

function(target_add_libraries TARGET)
  if("${ARGN}" MATCHES "[gG][lL][fF][wW]")
    target_add_glfw(${TARGET})
  endif()
  if("${ARGN}" MATCHES "[gG][lL][aA][dD]")
    target_add_glad(${TARGET})
  endif()
  if("${ARGN}" MATCHES "[sS][tT][bB]")
    target_add_stb(${TARGET})
  endif()
  if("${ARGN}" MATCHES "[gG][lL][mM]")
    target_add_glm(${TARGET})
  endif()
  if("${ARGN}}" MATCHES "[fF][rR][eE]{2}[tT][yY][pP][eE]")
    target_add_freetype(${TARGET})
  endif()
  if("${ARGN}" MATCHES "[gG]([oO]{2}[gG][lL][eE]){0,1}[_]{0,1}[tT][eE][sS][tT]")
    target_add_gtest(${TARGET})
  endif()
endfunction(target_add_libraries)
