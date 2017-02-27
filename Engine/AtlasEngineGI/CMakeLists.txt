set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")

project(AtlasEngineGI)

cmake_minimum_required(VERSION 2.8)
set(SOURCE_DIR src)
set(INCLUDE_DIR include)

file(GLOB_RECURSE SOURCES ${SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE HEADERS ${INCLUDE_DIR}/*.h)

set (INCLUDE_DIRS "")
foreach (_headerFile ${HEADERS})
    get_filename_component(_dir ${_headerFile} PATH)
    list (APPEND INCLUDE_DIRS ${_dir})
endforeach()
list(REMOVE_DUPLICATES INCLUDE_DIRS)

include_directories(${INCLUDE_DIRS})

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

#TinyXML 2 inclusion
include_directories(lib/tinyxml2)
add_subdirectory(lib/tinyxml2)

if(APPLE)
        set(CMAKE_CXX_FLAGS "-std=c++11 -g -ggdb -O0 -Wall -Wextra")
        #OpenGL inclusion
        find_package(OpenGL)
        include_directories(${OPENGL_INCLUDE_DIR})

        #Qt5 inclusion
        find_package(Qt5Core REQUIRED)
        find_package(Qt5Gui REQUIRED)
        find_package(Qt5Widgets REQUIRED)
        find_package(Qt5OpenGL REQUIRED)
        include_directories(${Qt5Widgets_INCLUDE_DIRS})
        add_definitions(${Qt5Widgets_DEFINITIONS})
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")

        add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

        qt5_use_modules(${PROJECT_NAME} Core Gui Widgets OpenGL)

        target_link_libraries(${PROJECT_NAME} ${QT_LIBRARIES} ${OPENGL_LIBRARIES} tinyxml2)
elseif(UNIX)
        set(CMAKE_CXX_FLAGS "-std=c++11 -g -ggdb -O0 -Wall -Wextra")
        find_package(OpenGL REQUIRED)
        find_package(Qt4 REQUIRED)
        set(Qt4_LIBRARIES Qt4::QtGui Qt4::QtCore Qt4::QtOpenGL)

        #OpenGL inclusion
        include_directories(${OPENGL_INCLUDE_DIR})

        include_directories(${Qt4Widgets_INCLUDE_DIRS})

        add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

        target_link_libraries(${PROJECT_NAME} ${Qt4_LIBRARIES} ${OPENGL_LIBRARIES} tinyxml2)
elseif(WIN32)
    set(CMAKE_CXX_FLAGS "-std=c++11 -g -ggdb -O0")
    #OpenGL inclusion
    find_package(OpenGL)
    include_directories(${OPENGL_INCLUDE_DIR})

    FILE(GLOB folder_shader  shaders/*.* shaders/*/*.*)

    #Qt5 inclusion
    set(CMAKE_PREFIX_PATH "D:\\Program Files\\Qt\\5.7\\msvc2015_64\\")
    find_package(Qt5Core)
    find_package(Qt5Gui)
    find_package(Qt5Widgets)
    find_package(Qt5OpenGL)
    include_directories(${Qt5Widgets_INCLUDE_DIRS})
    include_directories(${Qt5Gui_INCLUDE_DIRS})
    include_directories(${Qt5Core_INCLUDE_DIRS})
    include_directories(${Qt5OpenGL_INCLUDE_DIRS})
    add_definitions(${Qt5Widgets_DEFINITIONS})
    add_definitions(${Qt5Gui_DEFINITIONS})
    add_definitions(${Qt5Core_DEFINITIONS})
    add_definitions(${Qt5OpenGL_DEFINITIONS})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS} ${Qt5Core_EXECUTABLE_COMPILE_FLAGS} ${Qt5Gui_EXECUTABLE_COMPILE_FLAGS} ${Qt5OpenGL_EXECUTABLE_COMPILE_FLAGS}")

    #GLEW inclusion
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/glew)
    link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/msvs2015)
    set(GLEW_LIB lib/msvs2015/glew32)
    set(GLEW_STATIC_LIB lib/msvs2015/glew32s)

    add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${folder_shader})

    target_link_libraries(${PROJECT_NAME} Qt5::WinMain Qt5::Widgets Qt5::Gui Qt5::OpenGL ${QT_LIBRARIES} ${GLEW_LIB} ${GLEW_STATIC_LIB} ${OPENGL_LIBRARIES} tinyxml2)
endif()
