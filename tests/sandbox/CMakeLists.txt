file(GLOB_RECURSE SOURCES
  main.cpp
  "${PROJECT_SOURCE_DIR}/src/*.cpp"
  )

add_executable(sandbox ${SOURCES})
