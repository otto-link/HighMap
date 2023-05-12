file(GLOB_RECURSE SOURCES
  ex_stratify.cpp
  "${PROJECT_SOURCE_DIR}/src/*.cpp"
  )

add_executable(ex_stratify ${SOURCES})
