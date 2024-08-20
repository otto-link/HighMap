project(delaunator)

set(DELAUNATOR_DIR ${CMAKE_CURRENT_SOURCE_DIR}/delaunator/include)

add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} INTERFACE ${DELAUNATOR_DIR})
