project(NeuralNetworkEvaluation)
cmake_minimum_required(VERSION 2.8)

SET(CMAKE_CXX_FLAGS "-std=c++0x")

aux_source_directory(. SRC_LIST)
file(GLOB_RECURSE HEADERS ./*.h)
INCLUDE_DIRECTORIES(${HEADERS})

add_executable(${PROJECT_NAME} ${SRC_LIST} ${HEADERS})

target_link_libraries(${PROJECT_NAME})
