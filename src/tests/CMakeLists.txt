option(test "build all tests." ON)
if(test)
  configure_file(src/tests/test-config.h.in config/test-config.h)
  set(TEST_SOURCES ${SOURCES})
  list(FILTER TEST_SOURCES INCLUDE REGEX "${SRC_DIR}/tests/*")
  list(FILTER TEST_SOURCES INCLUDE REGEX ".*\.cpp")

  set(INC_SOURCES ${SOURCES})
  list(FILTER INC_SOURCES INCLUDE REGEX "${SRC_DIR}/inc/*")

  add_executable(runUnitTests ${TEST_SOURCES} ${INC_SOURCES})
  if(MSVC)
    target_compile_options(runUnitTests PRIVATE /W4 /WX)
  else()
    target_compile_options(runUnitTests  PRIVATE -Wall -Wextra -Wpedantic -Werror)
  endif()
  target_link_libraries(runUnitTests gtest gtest_main)
  target_add_libraries(runUnitTests "glm" "glad" "glfw" "gtest")
  target_include_directories(runUnitTests PRIVATE "${INC_DIR}")
  target_include_directories(runUnitTests PRIVATE "${SRC_DIR}/tests")
  target_include_directories(runUnitTests PRIVATE "${CONF_DIR}")

  add_test(NAME TEST COMMAND runUnitTests)

endif()
