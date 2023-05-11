file(GLOB_RECURSE SOURCES
  ex_gradient_norm.cpp
  "${PROJECT_SOURCE_DIR}/src/*.cpp"
  )

add_executable(ex_gradient_norm ${SOURCES})
