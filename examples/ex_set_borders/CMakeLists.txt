file(GLOB_RECURSE SOURCES
  ex_set_borders.cpp
  "${PROJECT_SOURCE_DIR}/src/*.cpp"
  )

add_executable(ex_set_borders ${SOURCES})
