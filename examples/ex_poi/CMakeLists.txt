project(ex_poi)

add_executable(${PROJECT_NAME} ex_poi.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE highmap)
