
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
physicsdebugrenderer.cpp
texture.cpp
tilerenderer.cpp
torch.cpp
world.cpp
item.cpp
quickbarinventory.cpp
timer.cpp

#cloudsystem.cpp
#sky.cpp

client/client.cpp
client/gui/chatdialog.cpp
client/gui/debugmenu.cpp
client/gui/debugsettings.cpp
client/gui/gui.cpp
client/gui/mainmenu.cpp
client/gui/optionsdialog.cpp
client/gui/quickbarmenu.cpp

client/gui/core/Shell.cpp
client/gui/core/ShellFileInterface.cpp
client/gui/core/ShellRenderInterfaceOpenGL.cpp
client/gui/core/SystemInterfaceSDL2.cpp

client/gui/decoratorspritesheet.cpp
client/gui/decoratorspritesheetinstancer.cpp

server/server.cpp
server/contactlistener.cpp

settings/settings.cpp

network/packet.cpp
${CMAKE_CURRENT_BINARY_DIR}/packet.pb.cc
)

include(FindPkgConfig)

add_executable(${EXECUTABLE_NAME} ${SRC})

find_program(PROTOC_EXEC NAME protoc
PATHS
/usr/bin/
/usr/local/bin
/opt/bin
)

if (NOT PROTOC_EXEC)
    message(FATAL_ERROR
    "ERROR: Unable to launch protoc. The 'protoc' binary is a required build-time dependency. If this message is in error, check the output of 'which protoc' maybe the search paths need fixed in this file."
    )
endif (NOT PROTOC_EXEC)

# ----------------------- Generate protoc (somefile.pb.cc and somefile.pb.h) from a given somefile.proto file and build against that. the 'protoc' binary has to exist, but apparently distros have 2 packages mostly, libprotobuf and protoc, both of which are needed  -----------------------------------------
add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/packet.pb.cc
  COMMAND protoc  -I=${CMAKE_CURRENT_SOURCE_DIR} --cpp_out=${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/packet.proto
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/packet.proto
)

add_custom_target(
  do_protoc
  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/packet.pb.cc
)

add_dependencies(${EXECUTABLE_NAME} do_protoc)

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
${BOX2D_INCLUDE_DIR}
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
${BOX2D_LIBRARY}
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

