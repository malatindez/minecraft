option(test "build all tests." ON)
if(test)

  # Unit Tests Add test cpp file
  file(GLOB_RECURSE TEST_SOURCES ${PROJECT_SOURCE_DIR}/tests *.*)
  list(FILTER TEST_SOURCES INCLUDE REGEX "${SRC_DIR}/tests/*")
  add_executable(runUnitTests ${TEST_SOURCES} ${INC_DIR})
  target_link_libraries(runUnitTests gtest gtest_main)
  target_add_libraries(runUnitTests "glm" "gtest")
  target_include_directories(runUnitTests PRIVATE "${INC_DIR}")

  add_test(NAME TEST COMMAND runUnitTests)

endif()
