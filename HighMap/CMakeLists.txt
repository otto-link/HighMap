cmake_minimum_required(VERSION 3.22)

project(
  highmap
  VERSION 0.0.1
  LANGUAGES CXX
  DESCRIPTION
    "C++ library to generate two-dimensional terrain heightmaps for software rendering or video games."
)

file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_include_directories(
  ${PROJECT_NAME}
  PUBLIC ${PROJECT_SOURCE_DIR}/include
  PRIVATE ${PROJECT_SOURCE_DIR}/include/internal)

set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})

target_link_libraries(
  ${PROJECT_NAME}
  assimp::assimp
  GSL::gsl
  GSL::gslcblas
  Eigen3::Eigen
  PNG::PNG
  cpp-spline::cpp-spline
  dkm::dkm
  FastNoiseLite::FastNoiseLite
  hmm::hmm
  libInterpolate::libInterpolate
  macro-logger::macro-logger
  NoiseLib
  OpenMP::OpenMP_CXX
  xsimd::xsimd)

if(HIGHMAP_ENABLE_OPENCL)
  target_link_libraries(${PROJECT_NAME} ${OpenCL_LIBRARIES})
endif(HIGHMAP_ENABLE_OPENCL)
