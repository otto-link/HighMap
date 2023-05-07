cmake_minimum_required(VERSION 3.22)

project(highmap)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Ofast -ffast-math -funroll-all-loops -funsafe-loop-optimizations -funsafe-math-optimizations -frounding-math -march=native")

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

file(GLOB SOURCES
  main.cpp
  "${PROJECT_SOURCE_DIR}/src/*.cpp"
  )

add_executable(main ${SOURCES})

target_include_directories(main
  PRIVATE 
  ${PROJECT_SOURCE_DIR}/include
  )
