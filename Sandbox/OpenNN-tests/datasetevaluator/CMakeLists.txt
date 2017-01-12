cmake_minimum_required(VERSION 3.2)
project(DataSetEvaluator)

set(SRC main.cpp)
add_executable(${PROJECT_NAME} ${SRC})

target_link_libraries(${PROJECT_NAME} opennn tinyxml2)
