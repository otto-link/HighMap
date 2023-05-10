file(GLOB_RECURSE SOURCES
  ex_perlin.cpp
  "${PROJECT_SOURCE_DIR}/src/*.cpp"
  )

add_executable(ex_perlin ${SOURCES})
