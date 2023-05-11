file(GLOB_RECURSE SOURCES
  ex_gain.cpp
  "${PROJECT_SOURCE_DIR}/src/*.cpp"
  )

add_executable(ex_gain ${SOURCES})
