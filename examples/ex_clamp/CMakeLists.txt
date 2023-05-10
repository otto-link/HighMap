file(GLOB_RECURSE SOURCES
  ex_clamp.cpp
  "${PROJECT_SOURCE_DIR}/src/*.cpp"
  )

add_executable(ex_clamp ${SOURCES})
