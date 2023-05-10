cmake_minimum_required(VERSION 3.24)

project(highmap)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Ofast -ffast-math -funroll-all-loops -funsafe-loop-optimizations -funsafe-math-optimizations -frounding-math -march=native")

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

include_directories(
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}/external/FastNoiseLite/include
  ${PROJECT_SOURCE_DIR}/external/stb_image/include
  )

add_subdirectory(${PROJECT_SOURCE_DIR}/doc)
add_subdirectory(${PROJECT_SOURCE_DIR}/examples)
add_subdirectory(${PROJECT_SOURCE_DIR}/tests)

