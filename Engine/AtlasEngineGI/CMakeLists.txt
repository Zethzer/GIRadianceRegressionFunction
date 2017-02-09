set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")

project(AtlasEngineGI)

cmake_minimum_required(VERSION 2.8)
set(SOURCE_DIR src)
set(INCLUDE_DIR include)

set(CMAKE_AUTOMOC ON)

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
        find_package(QtCore REQUIRED)
        find_package(QtGui REQUIRED)
        find_package(QtWidgets REQUIRED)
        find_package(QtOpenGL REQUIRED)
        include_directories(${QtWidgets_INCLUDE_DIRS})
	add_definitions(${QtWidgets_DEFINITIONS})
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

        add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

        target_link_libraries(${PROJECT_NAME} ${Qt4_LIBRARIES} ${OPENGL_LIBRARIES})
elseif(WIN32)
    set(CMAKE_CXX_FLAGS "-std=c++11 -g -ggdb -O0")
    #OpenGL inclusion
    find_package(OpenGL)
    include_directories(${OPENGL_INCLUDE_DIR})

    # Assimp
    set(ASSIMP64_LIB lib/msvs2015/assimp)
    set(ASSIMP32_LIB lib/msvs2015/assimp32)

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

    # ZlibStatic
    set(ZLIBSTATIC32_LIB lib/msvs2015/zlibstatic32)
    set(ZLIBSTATICD_LIB lib/msvs2015/zlibstaticd)

    add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
    target_link_libraries(${PROJECT_NAME} Qt5::WinMain Qt5::Widgets Qt5::Gui Qt5::OpenGL ${QT_LIBRARIES} ${GLEW_LIB} ${GLEW_STATIC_LIB} ${ASSIMP64_LIB} ${ASSIMP32_LIB} ${ZLIBSTATIC32_LIB} ${ZLIBSTATICD_LIB} ${OPENGL_LIBRARIES})
endif()
