file(GLOB_RECURSE SOURCES
  ex_gamma_correction.cpp
  "${PROJECT_SOURCE_DIR}/src/*.cpp"
  )

add_executable(ex_gamma_correction ${SOURCES})
