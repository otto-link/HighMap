file(GLOB_RECURSE SOURCES
  ex_convolve2d_svd.cpp
  "${PROJECT_SOURCE_DIR}/src/*.cpp"
  )

add_executable(ex_convolve2d_svd ${SOURCES})

find_package(GSL REQUIRED)
target_link_libraries(ex_convolve2d_svd GSL::gsl GSL::gslcblas)
