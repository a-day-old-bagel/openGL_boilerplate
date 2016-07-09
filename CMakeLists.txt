
# No lesser version of cMake may be used.
cmake_minimum_required(VERSION 3.3)

# Declare the project in question
project(OpenGL)

# Make sure the project compiles with the c++11 and thread safety flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra -g -O3")

# Tell cMake where to look for the modules that search for SDL2 (the others come with cMake)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmakeModules/")

## Tell cMake where to look for third party libs (add to this for your own builds)
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "C:/libs/SDL2-2.0.4" "C:/libs/glew-1.13.0" "C:/libs/freetype")
set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "C:/libs/SDL2-2.0.4/lib/x86" "C:/libs/glew-1.13.0/lib/Release/win32")

# Find SDL 2.0
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})
set(LIBS ${LIBS} ${SDL2_LIBRARY})

# Find GLEW
find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})
set(LIBS ${LIBS} ${GLEW_LIBRARIES})

# Find OpenGL
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
set(LIBS ${LIBS} ${OPENGL_LIBRARIES})

set(INCLUDE_DIR ./)
include_directories(${INCLUDE_DIR})

# Find all the source files
set(SOURCE_FILES main.cpp SimpleSquare.cpp SimpleSquare.h LoadShaders.cpp LoadShaders.h)

# Make an executable called DualityEngine out of all the above source files
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# Link to external libraries that were found above
target_link_libraries(${PROJECT_NAME} ${LIBS})