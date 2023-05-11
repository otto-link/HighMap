file(GLOB_RECURSE SOURCES
  ex_distance_transform.cpp
  "${PROJECT_SOURCE_DIR}/src/*.cpp"
  )

add_executable(ex_distance_transform ${SOURCES})
