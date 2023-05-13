file(GLOB_RECURSE SOURCES
  ex_convolve1d_ij.cpp
  "${PROJECT_SOURCE_DIR}/src/*.cpp"
  )

add_executable(ex_convolve1d_ij ${SOURCES})
