file(GLOB_RECURSE SOURCES
  ex_thermal.cpp
  "${PROJECT_SOURCE_DIR}/src/*.cpp"
  )

add_executable(ex_thermal ${SOURCES})
