file(GLOB_RECURSE SOURCES
  ex_multifractal_perlin.cpp
  "${PROJECT_SOURCE_DIR}/src/*.cpp"
  )

add_executable(ex_multifractal_perlin ${SOURCES})
