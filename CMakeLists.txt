cmake_minimum_required(VERSION 2.6)

# To force which compiler to use e.g. for me for clang instead of gcc, pass CC and CCX=/usr/bin/clang++ cmake .. ; ...

project(ore-infinium)

include(FeatureSummary)

# Enable debug symbols by default
if(CMAKE_BUILD_TYPE STREQUAL "")
  set(CMAKE_BUILD_TYPE Debug)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DDEBUG)

    set(CMAKE_CXX_FLAGS "-std=c++11 -O0 -g3 -fpermissive")
# -Wall -Wno-unused-variable -Wno-unused-function -Woverloaded-virtual -g3 -ggdb -ftrapv -fverbose-asm -Wextra -Wfloat-equal -Winit-self -Wuninitialized -Werror-implicit-function-declaration -Wundef -Wshadow  -Wpointer-arith -Wcast-align -Wstrict-overflow=5 -Wwrite-strings -Wswitch-default -Wconversion -Wno-switch -Wunreachable-code -Wformat=2 -fstack-check -Wdouble-promotion
    set(CMAKE_C_FLAGS "-Wall -Wno-unused-variable -Wno-unused-function -fpermissive -funroll-loops -fexpensive-optimizations -g3 -ggdb")
else(CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_CXX_FLAGS "-std=c++11 -O2 -Wall -Wno-unused-variable -Wno-unused-function -fexpensive-optimizations -Woverloaded-virtual")
    set(CMAKE_C_FLAGS "-O2 -Wall -Wno-unused-variable -Wno-unused-function -fexpensive-optimizations")
endif(CMAKE_BUILD_TYPE STREQUAL "Debug")

# (you can also set it on the command line: -D CMAKE_BUILD_TYPE=Release)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})

# Set version information in a config.h file
set(ore_infinium_VERSION_MAJOR 0)
set(ore_infinium_VERSION_MINOR 1)

find_package(SDL2 REQUIRED)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

find_package(FreeImage REQUIRED)
find_package(FreeType REQUIRED)
find_package(FTGL REQUIRED)

find_package(YamlCpp REQUIRED)

find_package(GLM REQUIRED)
find_package(LibRocket REQUIRED)
find_package(enet REQUIRED)

set_package_properties(SDL2 PROPERTIES DESCRIPTION "SDL2, used for creating an open source, cross-platform OpenGL window, audio and input" URL "http://hg.libsdl.org/SDL/")

set_package_properties(OpenGL PROPERTIES DESCRIPTION "OpenGL >= 3.0 is required. Version check is done at runtime" URL "-")
set_package_properties(GLEW PROPERTIES DESCRIPTION "OpenGL Extension Wrangler, to create our function pointers and extension loading" URL "http://glew.sourceforge.net/")

set_package_properties(FreeImage PROPERTIES DESCRIPTION "Cross-platform open source image loading library" URL "http://freeimage.sourceforge.net/")
set_package_properties(FTGL PROPERTIES DESCRIPTION "Easy, open source OpenGL font rendering library" URL "http://sourceforge.net/projects/ftgl/")
set_package_properties(FreeType PROPERTIES DESCRIPTION "FreeType >= 2" URL "http://sourceforge.net/projects/freetype/files/freetype2/")

set_package_properties(Yaml-Cpp PROPERTIES DESCRIPTION "YAML CPP, for config file and sprite sheet reading" URL "http://code.google.com/p/yaml-cpp/")

set_package_properties(GLM PROPERTIES DESCRIPTION "OpenGL Open Source Mathematics Library (for all our math, in a GLSL-like nature)" URL "http://glm.g-truc.net/")
set_package_properties(LibRocket PROPERTIES DESCRIPTION "LibRocket GUI library" URL "")
set_package_properties(enet PROPERTIES DESCRIPTION "enet UDP networking library" URL "http://enet.bespin.org/index.html")

message("\n----------------------------------------------------------------------------------------------")
feature_summary(WHAT REQUIRED_PACKAGES_FOUND DESCRIPTION "Successfully Enabled REQUIRED Features:\n")
message("\n----------------------------------------------------------------------------------------------")
feature_summary(WHAT REQUIRED_PACKAGES_NOT_FOUND DESCRIPTION "NOT FOUND REQUIRED FEATURES:\n")
message("\n----------------------------------------------------------------------------------------------")

include_directories(${CMAKE_SOURCE_DIR})

configure_file(
  "${PROJECT_SOURCE_DIR}/config.h.in"
  "${PROJECT_BINARY_DIR}/config.h"
  )

include_directories("${PROJECT_BINARY_DIR}")

add_subdirectory(src)

