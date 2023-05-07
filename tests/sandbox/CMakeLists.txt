file(GLOB_RECURSE SOURCES
  main.cpp
  "${PROJECT_SOURCE_DIR}/src/*.cpp"
  )

add_executable(sandbox ${SOURCES})

target_include_directories(sandbox
  PRIVATE 
  ${PROJECT_SOURCE_DIR}/include
  )
