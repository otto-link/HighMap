project(cpp-spline)

set(CPP_SPLINE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cpp-spline/spline/src/main/cpp)

set(CPP_SPLINE_SRC
    ${CPP_SPLINE_DIR}/Bezier.cpp ${CPP_SPLINE_DIR}/BSpline.cpp
    ${CPP_SPLINE_DIR}/CatmullRom.cpp ${CPP_SPLINE_DIR}/Curve.cpp
    ${CPP_SPLINE_DIR}/Vector.cpp)

set(CPP_SPLINE_INCLUDE_DIR ${CPP_SPLINE_DIR}/.)

add_library(${PROJECT_NAME} STATIC ${CPP_SPLINE_SRC})
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC ${CPP_SPLINE_INCLUDE_DIR})
