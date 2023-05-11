file(GLOB_RECURSE SOURCES
  ex_depression_filling.cpp
  "${PROJECT_SOURCE_DIR}/src/*.cpp"
  )

add_executable(ex_depression_filling ${SOURCES})
