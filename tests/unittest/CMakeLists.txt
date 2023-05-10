include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.12.0
)
FetchContent_MakeAvailable(googletest)

enable_testing()

file(GLOB_RECURSE SOURCES
  main.cpp
  "${PROJECT_SOURCE_DIR}/src/*.cpp"
  "${PROJECT_SOURCE_DIR}/tests/unittest/*.cpp"
  )
  
add_executable(
  unittest
  ${SOURCES}
  )
  
target_link_libraries(
  unittest
  gtest
  gtest_main
)

include(GoogleTest)
gtest_discover_tests(unittest)
