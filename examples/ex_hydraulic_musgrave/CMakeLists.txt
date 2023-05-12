file(GLOB_RECURSE SOURCES
  ex_hydraulic_musgrave.cpp
  "${PROJECT_SOURCE_DIR}/src/*.cpp"
  )

add_executable(ex_hydraulic_musgrave ${SOURCES})
