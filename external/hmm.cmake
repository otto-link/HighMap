project(hmm)

set(HMM_DIR ${CMAKE_CURRENT_SOURCE_DIR}/hmm/src)

set(HMM_SRC ${HMM_DIR}/base.cpp ${HMM_DIR}/blur.cpp ${HMM_DIR}/heightmap.cpp
            ${HMM_DIR}/triangulator.cpp)

set(HMM_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

add_library(${PROJECT_NAME} STATIC ${HMM_SRC})
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC ${HMM_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} glm::glm)

# target_compile_options(hmm PRIVATE -Wno-sign-compare -Wno-comment
# -Wno-type-limits)
