file(GLOB_RECURSE SOURCES
  ex_resample_to_shape.cpp
  "${PROJECT_SOURCE_DIR}/src/*.cpp"
  )

add_executable(ex_resample_to_shape ${SOURCES})
