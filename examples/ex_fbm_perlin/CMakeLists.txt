file(GLOB_RECURSE SOURCES
  ex_fbm_perlin.cpp
  "${PROJECT_SOURCE_DIR}/src/*.cpp"
  )

add_executable(ex_fbm_perlin ${SOURCES})
