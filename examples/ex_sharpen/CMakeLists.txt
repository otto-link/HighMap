file(GLOB_RECURSE SOURCES
  ex_sharpen.cpp
  "${PROJECT_SOURCE_DIR}/src/*.cpp"
  )

add_executable(ex_sharpen ${SOURCES})
