file(GLOB_RECURSE SOURCES
  ex_white_sparse.cpp
  "${PROJECT_SOURCE_DIR}/src/*.cpp"
  )

add_executable(ex_white_sparse ${SOURCES})
