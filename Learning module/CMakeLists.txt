cmake_minimum_required(VERSION 2.8)
project(LearningModule)

aux_source_directory(src SRC_LIST)
file(GLOB_RECURSE HEADERS src/*.h)
include_directories(${HEADERS})

set (CMAKE_CXX_STANDARD 11)
SET(CPACK_GENERATOR "TGZ")

add_subdirectory(tinyxml2)
add_subdirectory(opennn)
include_directories(opennn)

set(CMAKE_CXX_FLAGS "-std=c++11 -g -ggdb -O0 -Wall -Wextra")

add_executable(${PROJECT_NAME} ${SRC_LIST} ${HEADERS})

target_link_libraries(${PROJECT_NAME} opennn tinyxml2)

include(CPack)
