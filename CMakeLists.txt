cmake_minimum_required(VERSION 2.6)

project(ore-chasm)

#project (ore_chasm)

# Enable debug symbols by default
if(CMAKE_BUILD_TYPE STREQUAL "")
  set(CMAKE_BUILD_TYPE Debug)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DDEBUG)

    #  cmake has this annoying fucking bug that took me a while to figure out how i can use clang: http://www.cmake.org/pipermail/cmake/2011-February/042629.html

    set(CMAKE_CXX_FLAGS "-std=c++11 -O0 -g3 -fpermissive")
# -Wall -Wno-unused-variable -Wno-unused-function -Woverloaded-virtual -g3 -ggdb -ftrapv -fverbose-asm -Wextra -Wfloat-equal -Winit-self -Wuninitialized -Werror-implicit-function-declaration -Wundef -Wshadow  -Wpointer-arith -Wcast-align -Wstrict-overflow=5 -Wwrite-strings -Wswitch-default -Wconversion -Wno-switch -Wunreachable-code -Wformat=2 -fstack-check -Wdouble-promotion
    set(CMAKE_C_FLAGS "-Wall -Wno-unused-variable -Wno-unused-function -fpermissive -funroll-loops -fexpensive-optimizations -g3 -ggdb")
else(CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_CXX_FLAGS "-std=c++11 -O2 -Wall -Wno-unused-variable -Wno-unused-function -fexpensive-optimizations -Woverloaded-virtual")
    set(CMAKE_C_FLAGS "-O2 -Wall -Wno-unused-variable -Wno-unused-function -fexpensive-optimizations")
endif(CMAKE_BUILD_TYPE STREQUAL "Debug")

#set(CMAKE_C_COMPILER "/usr/bin/clang")
#set(CMAKE_CXX_COMPILER "/usr/bin/clang++")

# (you can also set it on the command line: -D CMAKE_BUILD_TYPE=Release)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})

# Set version information in a config.h file
set(ore_chasm_VERSION_MAJOR 0)
set(ore_chasm_VERSION_MINOR 1)

find_package(SDL2 REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(FreeImage REQUIRED)
find_package(PugiXML REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(FreeType REQUIRED)
find_package(FTGL REQUIRED)
find_package(GLM REQUIRED)

if (NOT SDL2_FOUND)
    message(FATAL_ERROR "Could not find SDL2. Install that shit, yo.")
endif()

if (NOT SDLMIXER_FOUND)
    message(FATAL_ERROR "Could not find SDL2_mixer addon. Install that shit, yo.")
endif()

if (NOT FREEIMAGE_FOUND)
    message(FATAL_ERROR "Could not find Free Image lib. Unable to load images/textures. Install that shit, yo.")
endif()

if (NOT FREETYPE_FOUND)
    message(FATAL_ERROR "Could not find FreeType. Install that shit, yo.")
endif()

if (NOT FTGL_FOUND)
    message(FATAL_ERROR "Could not find FTGL. We use it to render fonts easily, because I'm lazy. Install that shit, yo.")
endif()

if (NOT GLM_FOUND)
    message(FATAL_ERROR "Could not find GLM. Used for vector/matrix math. Install that shit, yo.")
endif()

if (NOT PugiXML_FOUND)
    message(FATAL_ERROR "Could not find pugixml library.")
endif()

include_directories( ${CMAKE_SOURCE_DIR} ${PugiXML_INCLUDE_DIR})

configure_file(
  "${PROJECT_SOURCE_DIR}/config.h.in"
  "${PROJECT_BINARY_DIR}/config.h"
  )

include_directories("${PROJECT_BINARY_DIR}")

add_subdirectory(src)

