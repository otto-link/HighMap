cmake_minimum_required(VERSION 3.22)

project(highmap-root)

option(HIGHMAP_ENABLE_OPENCL "" OFF)
option(HIGHMAP_ENABLE_DOCS "" ON)
option(HIGHMAP_ENABLE_EXAMPLES "" ON)
option(HIGHMAP_ENABLE_TESTS "" ON)

set(CMAKE_CXX_STANDARD 20)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  # MSVC compiler options
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /Od")
else()
  # GNU compiler options
  set(CMAKE_CXX_FLAGS
      "${CMAKE_CXX_FLAGS} -Wall -Wextra -O3 -pthread -DLOG_LEVEL=3 -Wno-free-nonheap-object"
  )
endif()

add_definitions("-DBOOST_ALLOW_DEPRECATED_HEADERS")
add_definitions("-DBOOST_BIND_GLOBAL_PLACEHOLDERS")

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

# --- dependencies

find_package(Eigen3 REQUIRED)
find_package(GSL REQUIRED COMPONENTS gsl gslcblas)
find_package(PNG REQUIRED) # libpng

if(HIGHMAP_ENABLE_OPENCL)
  find_package(OpenCL REQUIRED)
endif(HIGHMAP_ENABLE_OPENCL)

add_subdirectory(external)

# --- library

add_subdirectory(HighMap)

# --- everything else...

if(HIGHMAP_ENABLE_DOCS)
  add_subdirectory(${PROJECT_SOURCE_DIR}/doc)
endif(HIGHMAP_ENABLE_DOCS)

if(HIGHMAP_ENABLE_EXAMPLES)
  if(HIGHMAP_ENABLE_OPENCL)
    set_directory_properties(PROPERTIES COMPILE_DEFINITIONS ENABLE_OPENCL=1)
  endif(HIGHMAP_ENABLE_OPENCL)
  add_subdirectory(${PROJECT_SOURCE_DIR}/examples)
endif(HIGHMAP_ENABLE_EXAMPLES)

if(HIGHMAP_ENABLE_TESTS)
  add_subdirectory(${PROJECT_SOURCE_DIR}/tests)
endif(HIGHMAP_ENABLE_TESTS)
