cmake_minimum_required(VERSION 3.18.0)

project(Noise VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Activate OpenMP
find_package(OpenMP REQUIRED)

# Add CMake recipes
list(PREPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/")

# Set the project as startup project in Visual Studio
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Noise)

add_library(NoiseLib STATIC)

message(STATUS "Creating target 'NoiseLib'")

set(HEADER_FILES
    include/controlfunction.h
    #include/imagecontrolfunction.h
    #include/lichtenbergcontrolfunction.h
    include/math2d.h
    include/math3d.h
    include/noise.h
    include/perlin.h
    #include/perlincontrolfunction.h
    #include/planecontrolfunction.h
    include/spline.h
    include/utils.h
)

set(SRC_FILES
    #source/imagecontrolfunction.cpp
    source/math2d.cpp
    source/math3d.cpp
    source/perlin.cpp
    source/spline.cpp
    source/utils.cpp
)

# Setup filters in Visual Studio
source_group("Header Files" FILES ${HEADER_FILES})
source_group("Source Files" FILES ${SRC_FILES})

target_sources(NoiseLib
    PUBLIC
    ${HEADER_FILES}
    PRIVATE
    ${SRC_FILES}
)

target_include_directories(NoiseLib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(NoiseLib 
    PUBLIC
    OpenMP::OpenMP_CXX
)
