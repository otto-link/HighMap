file(GLOB_RECURSE SOURCES
  ex_remap.cpp
  "${PROJECT_SOURCE_DIR}/src/*.cpp"
  )

add_executable(ex_remap ${SOURCES})
