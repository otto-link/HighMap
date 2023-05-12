file(GLOB_RECURSE SOURCES
  ex_sediment_deposition.cpp
  "${PROJECT_SOURCE_DIR}/src/*.cpp"
  )

add_executable(ex_sediment_deposition ${SOURCES})
