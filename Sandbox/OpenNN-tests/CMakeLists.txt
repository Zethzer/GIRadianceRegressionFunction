cmake_minimum_required(VERSION 3.2)

set (CMAKE_CXX_STANDARD 11)
SET(CPACK_GENERATOR "TGZ")

set(CMAKE_CXX_FLAGS "-std=c++11 -g -ggdb -O0 -Wall -Wextra")
include(CPack)


add_subdirectory(tinyxml2)
add_subdirectory(opennn)
include_directories(opennn)

add_subdirectory(datasetevaluator)
add_subdirectory(datasetgenerator)
add_subdirectory(datasetcomparator)
