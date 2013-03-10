
# Define sources and executable
set(EXECUTABLE_NAME "ore-infinium")

set(SRC
block.cpp
camera.cpp
chunk.cpp
collisionmap.cpp
debug.cpp
entity.cpp
game.cpp
image.cpp
lightrenderer.cpp
main.cpp
player.cpp
shader.cpp
sprite.cpp
spritesheetrenderer.cpp
texture.cpp
tilerenderer.cpp
torch.cpp
world.cpp
item.cpp

#cloudsystem.cpp
#sky.cpp

client/client.cpp
client/gui/chatdialog.cpp
client/gui/debugmenu.cpp
client/gui/gui.cpp
client/gui/mainmenu.cpp
client/gui/optionsdialog.cpp
client/gui/quickbarmenu.cpp

client/gui/core/Shell.cpp
client/gui/core/ShellFileInterface.cpp
client/gui/core/ShellRenderInterfaceOpenGL.cpp
client/gui/core/SystemInterfaceSDL2.cpp

server/server.cpp

settings/settings.cpp

network/packet.cpp
#${CMAKE_CURRENT_BINARY_DIR}/packet.pb.cc
network/protobuf-compiled/packet.pb.cc
)

include(FindPkgConfig)

add_executable(${EXECUTABLE_NAME} ${SRC})

#message("PROJ BIN DITR: " + ${CMAKE_SOURCE_DIR} + "/src/network/protobuf-src")
## protoc -I=protobuf-src --cpp_out=protobuf-compiled protobuf-src/packet.proto
##kexecute_process("protoc -I=" + ${PROJECT_BINARY_DIR} + " --cpp out=")
##
#set (NETWORKPATH ${CMAKE_SOURCE_DIR}/src/network)
#set (PROTOBUF_SRC_GEN ${NETWORKPATH}/protobuf-src)
#set (PROTOBUF_COMPILED_GEN ${NETWORKPATH}/protobuf-compiled)
##
#set(PROTOBUF_CMD_ARGS "-I=" "${PROTOBUF_SRC_GEN} --cpp_out=${PROTOBUF_COMPILED_GEN}" " ${PROTOBUF_SRC_GEN}/packet.proto")
#message(${PROTOBUF_CMD_ARGS})
#

#execute_process(COMMAND "${PROTOBUF_CMD_ARGS}" RESULT_VARIABLE result)
#message("GENERATED PROTOBUF FILES, result: " ${result})
#

message(${CMAKE_CURRENT_BINARY_DIR})
message(${CMAKE_CURRENT_SOURCE_DIR})

#message(somemessage protoc -I=${CMAKE_CURRENT_SOURCE_DIR} --cpp_out=${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/packet.proto DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/packet.proto)

#add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/packet.cc
#COMMAND protoc -I=${CMAKE_CURRENT_SOURCE_DIR} --cpp_out=${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/packet.proto DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/packet.proto)

#add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/packet.h
#COMMAND "protoc -I=" #${CMAKE_CURRENT_SOURCE_DIR} "--cpp_out=${CMAKE_CURRENT_BINARY_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/network/protobuf-src/packet.proto"
#DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/packet.proto
#)
#
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/packet.h
COMMAND protoc
 DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/packet.h.in
)
# -I=${CMAKE_CURRENT_SOURCE_DIR} --cpp_out=${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/packet.proto
 add_dependencies(ore-infinium ${CMAKE_CURRENT_BINARY_DIR}/packet.h)

include_directories(
${CMAKE_SOURCE_DIR}
${SDL2_INCLUDE_DIR}
${SDLMIXER_INCLUDE_DIR}
${FREEIMAGE_INCLUDE_PATH}
${FREETYPE_INCLUDE_DIRS}
${GLM_INCLUDE_DIRS}
${YAMLCPP_INCLUDE_DIR}
${GLEW_INCLUDE_PATH}
${LIBROCKET_INCLUDE_DIRS}
${ENET_INCLUDE_DIRS}
${PROTOBUF_INCLUDE_DIR}
)

target_link_libraries(
${EXECUTABLE_NAME}
${SDL2_LIBRARY}
${SDL2MAIN_LIBRARY}
${SDLMIXER_LIBRARY}
${FREEIMAGE_LIBRARY}
${FREETYPE_LIBRARY}
${OPENGL_LIBRARIES}
${GLEW_LIBRARY}
${LIBROCKET_LIBRARIES}
${LIBROCKET_DEBUGGER_LIBRARY}
${LIBROCKET_CONTROLS_LIBRARY}
${YAMLCPP_LIBRARY}
${ENET_LIBRARIES}
${PROTOBUF_LIBRARY}
)

# CPack packaging
include(InstallRequiredSystemLibraries)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/COPYING")
set(CPACK_PACKAGE_VERSION_MAJOR "${ore_infinium_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${ore_infinium_VERSION_MINOR}")
include(CPack)

#add_subdirectory(${CMAKE_SOURCE_DIR}/src/)

# Install target
install(DIRECTORY ../textures/ DESTINATION "textures") #FILES_MATCHING PATTERN "*")
install(DIRECTORY ../font/ DESTINATION "font") #FILES_MATCHING PATTERN "*")
install(DIRECTORY ./client/ DESTINATION "client") #FILES_MATCHING PATTERN "*")

install(DIRECTORY ./ DESTINATION bin FILES_MATCHING PATTERN "*.frag" )
install(DIRECTORY ./ DESTINATION bin FILES_MATCHING PATTERN "*.vert" )
install(TARGETS ${EXECUTABLE_NAME} DESTINATION bin)

