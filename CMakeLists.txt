cmake_minimum_required(VERSION 2.6)

project (ore_chasm)

# Enable debug symbols by default
if(CMAKE_BUILD_TYPE STREQUAL "")
  set(CMAKE_BUILD_TYPE Debug)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DDEBUG)

    #  cmake has this annoying fucking bug that took me a while to figure out how i can use clang: http://www.cmake.org/pipermail/cmake/2011-February/042629.html

    set(CMAKE_CXX_FLAGS "-std=c++11 -O0 -g3 -fpermissive")
# -Wall -Wno-unused-variable -Wno-unused-function -funroll-loops -Woverloaded-virtual -g3 -ggdb -ftrapv -fverbose-asm -Wextra -Wfloat-equal -Winit-self -Wuninitialized -Werror-implicit-function-declaration -Wundef -Wshadow  -Wpointer-arith -Wcast-align -Wstrict-overflow=5 -Wwrite-strings -Wswitch-default -Wconversion -Wno-switch -Wunreachable-code -Wformat=2 -fstack-check -Wdouble-promotion
    set(CMAKE_C_FLAGS "-Wall -Wno-unused-variable -Wno-unused-function -fpermissive -funroll-loops -fexpensive-optimizations -g3 -ggdb")
else(CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_CXX_FLAGS "-std=c++11 -O2 -Wall -Wno-unused-variable -Wno-unused-function -fexpensive-optimizations -Woverloaded-virtual")
    set(CMAKE_C_FLAGS "-O2 -Wall -Wno-unused-variable -Wno-unused-function -fexpensive-optimizations")
endif(CMAKE_BUILD_TYPE STREQUAL "Debug")

# FIXME because i don't know how to switch it easily
# and passing it to cmake on cli goes into an infinite loop
set(CMAKE_C_COMPILER "/usr/bin/clang")
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")

# (you can also set it on the command line: -D CMAKE_BUILD_TYPE=Release)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})
message("PATH: " + ${CMAKE_MODULE_PATH})
# Set version information in a config.h file
set(ore_chasm_VERSION_MAJOR 0)
set(ore_chasm_VERSION_MINOR 1)

find_package(Eigen3 3.1.2 REQUIRED)

if (NOT EIGEN3_FOUND)
    message(FATAL_ERROR "Can't find Eigen 3 library")
endif(NOT EIGEN3_FOUND)

find_package(SDL2 REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(PugiXML REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(FreeType REQUIRED)

if (NOT SDL2_FOUND)
    message(FATAL_ERROR "Could not find SDL2. Install that shit, yo.")
endif()

if (NOT SDLMIXER_FOUND)
    message(FATAL_ERROR "Could not find SDL2_mixer addon. Install that shit, yo.")
endif()

if (NOT SDL2IMAGE_FOUND)
    message(FATAL_ERROR "Could not find SDL2_image addon. Install that shit, yo.")
endif()

if (NOT FREETYPE_FOUND)
    message(FATAL_ERROR "Could not find FreeType. Install that shit, yo.")
endif()

if (NOT FTGL_FOUND)
    message(FATAL_ERROR "Could not find FTGL. We use it to render fonts easily, because I'm lazy. Install that shit, yo.")
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

# Define sources and executable
set(EXECUTABLE_NAME "ore-chasm")

set(SRC
src/main.cpp
src/game.cpp
src/debug.cpp
#src/world.cpp
#src/resourcemanager.cpp
#src/player.cpp
#src/texture.cpp
#src/entity.cpp
#src/block.cpp
#src/cloudsystem.cpp
#src/sky.cpp
)

include(FindPkgConfig)

add_executable(${EXECUTABLE_NAME} ${SRC})

include_directories(${CMAKE_SOURCE_DIR} ${SDL2_INCLUDE_DIR} ${SDL2IMAGE_INCLUDE_DIR} ${SDLMIXER_INCLUDE_DIR} ${FREETYPE_INCLUDE_DIRS} ${FTGL_INCLUDE_DIR} ${EIGEN3_INCLUDE_DIR} ${GLEW_INCLUDE_DIRS})
target_link_libraries(${EXECUTABLE_NAME} ${SDL2_LIBRARY} ${SDL2MAIN_LIBRARY} ${SDL2IMAGE_LIBRARY} ${SDLMIXER_LIBRARY} ${FREETYPE_LIBRARY} ${FTGL_LIBRARY} ${OPENGL_LIBRARIES} ${PugiXML_LIBRARY} ${GLEW_LIBRARIES})

# CPack packaging
include(InstallRequiredSystemLibraries)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/COPYING")
set(CPACK_PACKAGE_VERSION_MAJOR "${ore_chasm_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${ore_chasm_VERSION_MINOR}")
include(CPack)

add_subdirectory(${CMAKE_SOURCE_DIR}/src/)

# Install target
install(DIRECTORY textures/ DESTINATION "textures") #FILES_MATCHING PATTERN "*")
install(DIRECTORY font/ DESTINATION "font") #FILES_MATCHING PATTERN "*")

install(DIRECTORY src/ DESTINATION bin FILES_MATCHING PATTERN "*.frag" )
install(TARGETS ${EXECUTABLE_NAME} DESTINATION bin)
