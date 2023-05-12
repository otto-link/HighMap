file(GLOB_RECURSE SOURCES
  ex_flow_accumulation_d8.cpp
  "${PROJECT_SOURCE_DIR}/src/*.cpp"
  )

add_executable(ex_flow_accumulation_d8 ${SOURCES})
