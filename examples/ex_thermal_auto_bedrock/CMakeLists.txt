file(GLOB_RECURSE SOURCES
  ex_thermal_auto_bedrock.cpp
  "${PROJECT_SOURCE_DIR}/src/*.cpp"
  )

add_executable(ex_thermal_auto_bedrock ${SOURCES})
