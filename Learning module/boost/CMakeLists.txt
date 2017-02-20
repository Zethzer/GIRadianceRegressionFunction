file(GLOB_RECURSE SOURCES ./*.hpp)
file(GLOB_RECURSE HEADERS ./*.hpp)

set (INCLUDE_DIRS "")
foreach (_headerFile ${HEADERS})
    get_filename_component(_dir ${_headerFile} PATH)
    list (APPEND INCLUDE_DIRS ${_dir})
endforeach()
list(REMOVE_DUPLICATES INCLUDE_DIRS)

include_directories(${INCLUDE_DIRS})

add_library(boost ${SOURCES})

set_target_properties(boost PROPERTIES LINKER_LANGUAGE CXX)

include(FindOpenMP)
if(OPENMP_FOUND)
    message("OPENMP FOUND")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")

    target_link_libraries(boost gomp)
else()
    message("OPENMP NOT FOUND")

    target_link_libraries(boost)
endif()
