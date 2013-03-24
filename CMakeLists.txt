cmake_minimum_required(VERSION 2.8.6)

# To force which compiler to use e.g. for me for clang instead of gcc, pass CC and CCX=/usr/bin/clang++ cmake .. ; ...

project(ore-infinium)

include(FeatureSummary)
include(ExternalProject)

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

include(FindProtobuf)

find_package(SDL2)

find_package(OpenGL)
find_package(GLEW)

find_package(FreeImage)

find_package(YamlCpp)

find_package(Box2D)

find_package(GLM)
find_package(LibRocket)
find_package(enet)
find_package(Protobuf)

set_package_properties(SDL2 PROPERTIES DESCRIPTION "SDL2" URL "http://hg.libsdl.org/SDL/")
set_package_properties(SDL2 PROPERTIES TYPE REQUIRED PURPOSE "Used for setting up an OpenGL 3.0+ context as well as for input.")

set_package_properties(OpenGL PROPERTIES DESCRIPTION "OpenGL >= 3.0 is required. Version check is done at runtime" URL "-")
set_package_properties(OpenGL PROPERTIES TYPE REQUIRED PURPOSE "OpenGL is used for all rendering, without it you won't see anything :)")

set_package_properties(GLEW PROPERTIES DESCRIPTION "OpenGL Extension Wrangler" URL "http://glew.sourceforge.net/")
set_package_properties(GLEW PROPERTIES TYPE REQUIRED PURPOSE "To create our function pointers and extension loading for OpenGL")

set_package_properties(GLM PROPERTIES DESCRIPTION "OpenGL Open Source Mathematics Library" URL "http://glm.g-truc.net/")
set_package_properties(GLM PROPERTIES TYPE REQUIRED PURPOSE "For matrix transformations, for camera and position setup, etc.")

set_package_properties(FreeImage PROPERTIES DESCRIPTION "Cross-platform open source image loading library" URL "http://freeimage.sourceforge.net/")
set_package_properties(FreeImage PROPERTIES TYPE REQUIRED PURPOSE "For loading textures from disk")

set_package_properties(Box2D PROPERTIES DESCRIPTION "Box2D open source 2D iterative solver physics engine" URL "")
set_package_properties(Box2D PROPERTIES TYPE REQUIRED PURPOSE "I use it for everything collision-wise in the game.")

set_package_properties(LibRocket PROPERTIES DESCRIPTION "LibRocket Cross-Platform GUI library that is described with CSS and HTML" URL "")
set_package_properties(LibRocket PROPERTIES TYPE REQUIRED PURPOSE "For our game's user interface (inventory, game menu, etc.)")

set_package_properties(enet PROPERTIES DESCRIPTION "enet reliabale UDP networking library" URL "http://enet.bespin.org/index.html")
set_package_properties(enet PROPERTIES TYPE REQUIRED PURPOSE "For networking, obviously.")

set_package_properties(Protobuf PROPERTIES DESCRIPTION "Google's protobuf Protocol Buffers implementation" URL "http://code.google.com/p/protobuf/")
set_package_properties(Protobuf PROPERTIES TYPE REQUIRED PURPOSE "For serializing and deserializing data in a cross-architecture/platform manner. Used for network communication as well as saving game files and such.")

set_package_properties(YamlCpp PROPERTIES DESCRIPTION "YAML CPP < 0.5, >= 0.30" URL "http://code.google.com/p/yaml-cpp/")
set_package_properties(YamlCpp PROPERTIES TYPE REQUIRED PURPOSE "For config file and sprite sheet reading")

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

include_directories(${CMAKE_SOURCE_DIR})

configure_file(
  "${PROJECT_SOURCE_DIR}/config.h.in"
  "${PROJECT_BINARY_DIR}/config.h"
  )

include_directories("${PROJECT_BINARY_DIR}")


#message("PROJ BIN DITR: " + ${CMAKE_SOURCE_DIR} + "/src/network/protobuf-src")
## protoc -I=protobuf-src --cpp_out=protobuf-compiled protobuf-src/packet.proto
##kexecute_process("protoc -I=" + ${PROJECT_BINARY_DIR} + " --cpp out=")
#
#set (NETWORKPATH ${CMAKE_SOURCE_DIR}/src/network)
#set (PROTOBUF_SRC_GEN ${NETWORKPATH}/protobuf-src)
#set (PROTOBUF_COMPILED_GEN ${NETWORKPATH}/protobuf-compiled)
#
#set(PROTOBUF_CMD_ARGS "protoc -I=" "${PROTOBUF_SRC_GEN} --cpp_out=${PROTOBUF_COMPILED_GEN}" " ${PROTOBUF_SRC_GEN}/packet.proto")
#
#message(${PROTOBUF_CMD_ARGS})
#execute_process(COMMAND "${PROTOBUF_CMD_ARGS}" RESULT_VARIABLE result)
#message("GENERATED PROTOBUF FILES, result: " ${result})
#
#add_custom_command(
#OUTPUT foo
#COMMAND protoc
#DEPENDS foo.in)
#)
#
#add_custom_target(process_protoc DEPENDS protoc)
#add_dependencies(main-target process_protoc)

add_subdirectory(src)
