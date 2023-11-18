cmake_minimum_required(VERSION 3.22)

project(highmap VERSION 0.0.1 LANGUAGES CXX DESCRIPTION "C++ library to generate two-dimensional terrain heightmaps for software rendering or video games.")

set(CMAKE_CXX_STANDARD 20)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O3 -pthread -DLOG_LEVEL=3")

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # MSVC compiler options
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /Od")
else()
    # GNU compiler options
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O3 -pthread -DLOG_LEVEL=3")
endif()

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

#--------------------------------------------------------------
# Library
#--------------------------------------------------------------

include(GNUInstallDirs)

file(GLOB_RECURSE SOURCES
     ${PROJECT_SOURCE_DIR}/src/*.cpp)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_include_directories(${PROJECT_NAME}
                           PUBLIC
                             ${PROJECT_SOURCE_DIR}/include
                           PRIVATE
                             ${PROJECT_SOURCE_DIR}/external/bezier/include/bezier
                             ${PROJECT_SOURCE_DIR}/external/dkm/include
                             # ${PROJECT_SOURCE_DIR}/external/FastNoise2/include/FastNoise
                             ${PROJECT_SOURCE_DIR}/external/FastNoiseLite/Cpp
                             ${PROJECT_SOURCE_DIR}/external/libInterpolate/src/libInterpolate
                             ${PROJECT_SOURCE_DIR}/external/macro-logger
                             ${PROJECT_SOURCE_DIR}/external/stb
                             ${PROJECT_SOURCE_DIR}/external/xsimd/include
                             ${PROJECT_SOURCE_DIR}/src)

set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})

find_package(GSL REQUIRED COMPONENTS gsl gslcblas)
target_link_libraries(${PROJECT_NAME} GSL::gsl GSL::gslcblas)

find_package(Eigen3 REQUIRED) # required by libInterpolate
include_directories(${EIGEN3_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} Eigen3::Eigen)

find_package(PNG REQUIRED) # libpng
target_link_libraries(${PROJECT_NAME} PNG::PNG)

# set(FASTNOISE2_NOISETOOL OFF CACHE BOOL "Build Noise Tool" FORCE)
# add_subdirectory(external/FastNoise2)
# target_compile_options(FastNoise PRIVATE -Wmaybe-uninitialized -Wunknown-pragmas)
# target_link_libraries(${PROJECT_NAME} FastNoise)

#--------------------------------------------------------------
# everything else...
#--------------------------------------------------------------

add_subdirectory(${PROJECT_SOURCE_DIR}/doc)
add_subdirectory(${PROJECT_SOURCE_DIR}/examples)
add_subdirectory(${PROJECT_SOURCE_DIR}/tests)
