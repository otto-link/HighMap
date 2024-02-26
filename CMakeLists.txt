cmake_minimum_required(VERSION 3.22)

project(
  highmap
  VERSION 0.0.1
  LANGUAGES CXX
  DESCRIPTION
    "C++ library to generate two-dimensional terrain heightmaps for software rendering or video games."
)

option(HIGHMAP_ENABLE_OPENCL "" OFF)
option(HIGHMAP_ENABLE_DOCS "" ON)
option(HIGHMAP_ENABLE_EXAMPLES "" ON)
option(HIGHMAP_ENABLE_TESTS "" ON)

set(CMAKE_CXX_STANDARD 20)

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O3 -pthread
# -DLOG_LEVEL=3")

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  # MSVC compiler options
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /Od")
else()
  # GNU compiler options
  set(CMAKE_CXX_FLAGS
      "${CMAKE_CXX_FLAGS} -Wall -Wextra -O3 -pthread -DLOG_LEVEL=3")
endif()

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

# --------------------------------------------------------------
# Library
# --------------------------------------------------------------

# add hmm sources (heightmap meshing)
set(HMM_DIR ${PROJECT_SOURCE_DIR}/external/hmm/src)
set(HMM_SRC
    ${HMM_DIR}/base.cpp ${HMM_DIR}/blur.cpp ${HMM_DIR}/heightmap.cpp
    # ${HMM_DIR}/stl.cpp
    ${HMM_DIR}/triangulator.cpp)

set(CPP_SPLINE_DIR
    ${PROJECT_SOURCE_DIR}/external/cpp-spline/spline/src/main/cpp)
set(CPP_SPLINE_SRC
    ${CPP_SPLINE_DIR}/Bezier.cpp ${CPP_SPLINE_DIR}/BSpline.cpp
    ${CPP_SPLINE_DIR}/CatmullRom.cpp ${CPP_SPLINE_DIR}/Curve.cpp
    ${CPP_SPLINE_DIR}/Vector.cpp)

file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)

add_library(${PROJECT_NAME} STATIC ${SOURCES} ${HMM_SRC} ${CPP_SPLINE_SRC})

if(HIGHMAP_ENABLE_OPENCL)
  message("OpenCL enabled")
  target_compile_definitions(${PROJECT_NAME} PRIVATE ENABLE_OPENCL=1)
else()
  message("OpenCL disabled")
endif(HIGHMAP_ENABLE_OPENCL)

target_include_directories(
  ${PROJECT_NAME}
  PUBLIC ${PROJECT_SOURCE_DIR}/include
         ${PROJECT_SOURCE_DIR}/external/macro-logger
         ${PROJECT_SOURCE_DIR}/external/FastNoiseLite/Cpp # public to retrieve
                                                          # FastNoiseLite enum
                                                          # in highmap headers
  PRIVATE ${PROJECT_SOURCE_DIR}/external/bezier/include/bezier
          ${PROJECT_SOURCE_DIR}/external/ # dendry & WavefrontObjWriter & hmm
          ${PROJECT_SOURCE_DIR}/external/cpp-spline/spline/src/main/cpp
          ${PROJECT_SOURCE_DIR}/external/dkm/include
          # ${PROJECT_SOURCE_DIR}/external/FastNoise2/include/FastNoise
          ${PROJECT_SOURCE_DIR}/external/libInterpolate/src/libInterpolate
          ${PROJECT_SOURCE_DIR}/external/xsimd/include
          ${PROJECT_SOURCE_DIR}/src)

include(GNUInstallDirs)

set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})

find_package(GSL REQUIRED COMPONENTS gsl gslcblas)
target_link_libraries(${PROJECT_NAME} GSL::gsl GSL::gslcblas)

find_package(Eigen3 REQUIRED) # required by libInterpolate
include_directories(${EIGEN3_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} Eigen3::Eigen)

find_package(PNG REQUIRED) # libpng
target_link_libraries(${PROJECT_NAME} PNG::PNG)

if(HIGHMAP_ENABLE_OPENCL)
  find_package(OpenCL REQUIRED)
  target_link_libraries(${PROJECT_NAME} ${OpenCL_LIBRARIES})
endif(HIGHMAP_ENABLE_OPENCL)

# set(FASTNOISE2_NOISETOOL OFF CACHE BOOL "Build Noise Tool" FORCE)
# add_subdirectory(external/FastNoise2) target_compile_options(FastNoise PRIVATE
# -Wmaybe-uninitialized -Wunknown-pragmas) target_link_libraries(${PROJECT_NAME}
# FastNoise)

# Dendry
add_subdirectory(external/dendry)
target_link_libraries(${PROJECT_NAME} NoiseLib)

# --------------------------------------------------------------
# everything else...
# --------------------------------------------------------------

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
