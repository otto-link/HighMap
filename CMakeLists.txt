cmake_minimum_required(VERSION 3.22)

project(highmap-root)

option(HIGHMAP_ENABLE_OPENCL "" ON)
option(HIGHMAP_ENABLE_DOCS "" ON)
option(HIGHMAP_ENABLE_EXAMPLES "" ON)
option(HIGHMAP_ENABLE_TESTS "" ON)

set(CMAKE_CXX_STANDARD 20)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  # MSVC compiler options
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /Od")

  message(STATUS "MSVC compiler...")
  add_definitions("-DHMAP_MSVC")
else()
  # GNU compiler options
  set(CMAKE_CXX_FLAGS
      "${CMAKE_CXX_FLAGS} -Wall -Wextra -O3 -Ofast -ffast-math -pthread -fPIC -DLOG_LEVEL=3 -Wno-free-nonheap-object -Wno-deprecated-enum-enum-conversion"
  )
endif()

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

# --- dependencies

find_package(GSL REQUIRED COMPONENTS gsl gslcblas)
find_package(assimp REQUIRED)
find_package(glm REQUIRED) # by hmm
find_package(OpenMP REQUIRED)
find_package(OpenCV REQUIRED)
find_package(OpenCL REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")

add_compile_definitions(CL_HPP_MINIMUM_OPENCL_VERSION=120
                        CL_HPP_TARGET_OPENCL_VERSION=120)

add_subdirectory(external)

# --- library

add_subdirectory(HighMap)

# --- everything else...

if(HIGHMAP_ENABLE_DOCS)
  message(STATUS "HighMap docs enabled")
  add_subdirectory(${PROJECT_SOURCE_DIR}/doc)
endif()

if(HIGHMAP_ENABLE_EXAMPLES)
  add_subdirectory(${PROJECT_SOURCE_DIR}/examples)
endif()

if(HIGHMAP_ENABLE_TESTS)
  message(STATUS "HighMap tests enabled")
  add_subdirectory(${PROJECT_SOURCE_DIR}/tests)
endif()
