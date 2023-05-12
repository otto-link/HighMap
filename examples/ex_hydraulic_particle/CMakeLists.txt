file(GLOB_RECURSE SOURCES
  ex_hydraulic_particle.cpp
  "${PROJECT_SOURCE_DIR}/src/*.cpp"
  )

add_executable(ex_hydraulic_particle ${SOURCES})
