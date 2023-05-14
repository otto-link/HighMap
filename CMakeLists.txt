cmake_minimum_required(VERSION 3.24)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Ofast -ffast-math -funroll-all-loops -funsafe-loop-optimizations -funsafe-math-optimizations -frounding-math -march=native -DLOG_LEVEL=3")

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

#--------------------------------------------------------------
# Library
#--------------------------------------------------------------

project(highmap VERSION 0.0.1 LANGUAGES CXX DESCRIPTION "C++ library to generate two-dimensional terrain heightmaps for software rendering or video games.")

include(GNUInstallDirs)

file(
  GLOB_RECURSE SOURCES
  main.cpp
  "${PROJECT_SOURCE_DIR}/src/*.cpp"
)

add_library(
  ${PROJECT_NAME} STATIC # SHARED
  ${SOURCES}
)

target_include_directories(
  ${PROJECT_NAME}
  PUBLIC
    ${PROJECT_SOURCE_DIR}/include
  PRIVATE
    ${PROJECT_SOURCE_DIR}/external/FastNoiseLite/include
    ${PROJECT_SOURCE_DIR}/external/macro-logger/include
    ${PROJECT_SOURCE_DIR}/external/stb_image/include
    ${PROJECT_SOURCE_DIR}/src
)

set_target_properties(
  ${PROJECT_NAME} PROPERTIES
  VERSION ${PROJECT_VERSION}
)

find_package(GSL REQUIRED)
target_link_libraries(${PROJECT_NAME} GSL::gsl GSL::gslcblas)

#--------------------------------------------------------------
# everything else...
#--------------------------------------------------------------

add_subdirectory(${PROJECT_SOURCE_DIR}/doc)
add_subdirectory(${PROJECT_SOURCE_DIR}/examples)
add_subdirectory(${PROJECT_SOURCE_DIR}/tests)