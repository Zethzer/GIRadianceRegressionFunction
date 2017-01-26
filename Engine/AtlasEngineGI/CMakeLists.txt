set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")

project(AtlasEngineGI)

cmake_minimum_required(VERSION 2.8)
set(SOURCE_DIR src)
set(INCLUDE_DIR include)

#file(GLOB_RECURSE INCLUDE ${INCLUDE_DIR}/*)

file(GLOB_RECURSE SOURCES ${SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE HEADERS ${INCLUDE_DIR}/*.h)

set (INCLUDE_DIRS "")
foreach (_headerFile ${HEADERS})
    get_filename_component(_dir ${_headerFile} PATH)
    list (APPEND INCLUDE_DIRS ${_dir})
endforeach()
list(REMOVE_DUPLICATES INCLUDE_DIRS)

include_directories(${INCLUDE_DIRS})

#aux_source_directory(${SOURCE_DIR} SRC_LIST)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

if(APPLE)
	set(CMAKE_CXX_FLAGS "-std=c++11 -g -ggdb -O0 -Wall -Wextra")
	#OpenGL inclusion
	find_package(OpenGL)
	include_directories(${OPENGL_INCLUDE_DIR})

	#Qt5 inclusion
	find_package(Qt5Core)
	find_package(Qt5Gui)
	find_package(Qt5Widgets)
	find_package(Qt5OpenGL)
        include_directories(${Qt5Widgets_INCLUDE_DIRS})
	add_definitions(${Qt5Widgets_DEFINITIONS})
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")

        add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

	qt5_use_modules(${PROJECT_NAME} Core Gui Widgets OpenGL)

        target_link_libraries(${PROJECT_NAME} ${QT_LIBRARIES} ${OPENGL_LIBRARIES})
elseif(UNIX)
        set(CMAKE_CXX_FLAGS "-std=c++11 -g -ggdb -O0 -Wall -Wextra")
        find_package(OpenGL REQUIRED)
        find_package(Qt4 REQUIRED)
        set(Qt4_LIBRARIES Qt4::QtGui Qt4::QtCore Qt4::QtOpenGL)

        #OpenGL inclusion
        include_directories(${OPENGL_INCLUDE_DIR})

        include_directories(${Qt5Widgets_INCLUDE_DIRS})

        add_executable(${PROJECT_NAME} ${SRC_LIST})

        target_link_libraries(${PROJECT_NAME} ${Qt4_LIBRARIES} ${OPENGL_LIBRARIES} libassimp.so)
elseif(WIN32)
    set(CMAKE_CXX_FLAGS "-std=c++11 -g -ggdb -O0 -Wall -Wextra")
    #OpenGL inclusion
    find_package(OpenGL)
    include_directories(${OPENGL_INCLUDE_DIR})

    #Qt5 inclusion
    find_package(Qt5Core)
    find_package(Qt5Gui)
    find_package(Qt5Widgets)
    find_package(Qt5OpenGL)
    include_directories(${Qt5Widgets_INCLUDE_DIRS})
    add_definitions(${Qt5Widgets_DEFINITIONS})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")

    #GLEW inclusion
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/glew)
    link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/glew)

    add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

    qt5_use_modules(${PROJECT_NAME} Core Gui Widgets OpenGL)

    target_link_libraries(${PROJECT_NAME} ${QT_LIBRARIES} ${OPENGL_LIBRARIES})
endif()
