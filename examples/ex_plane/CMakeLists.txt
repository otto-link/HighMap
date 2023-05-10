file(GLOB_RECURSE SOURCES
  ex_plane.cpp
  "${PROJECT_SOURCE_DIR}/src/*.cpp"
  )

add_executable(ex_plane ${SOURCES})
