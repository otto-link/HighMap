file(GLOB_RECURSE SOURCES
  ex_array.cpp
  "${PROJECT_SOURCE_DIR}/src/*.cpp"
  )

add_executable(ex_array ${SOURCES})

target_include_directories(ex_array
  PRIVATE 
  ${PROJECT_SOURCE_DIR}/include
  )
