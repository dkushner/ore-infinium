
# Define sources and executable
set(EXECUTABLE_NAME "ore-infinium")

set(SRC
main.cpp
game.cpp
debug.cpp
spritesheetmanager.cpp
shader.cpp
fontmanager.cpp
camera.cpp
sprite.cpp
block.cpp
world.cpp
player.cpp
entity.cpp
tilerenderer.cpp
image.cpp
gui/gui.cpp
gui/core/Shell.cpp
gui/core/ShellFileInterface.cpp
gui/core/ShellRenderInterfaceOpenGL.cpp
gui/core/SystemInterfaceSDL2.cpp
settings/settings.cpp
#cloudsystem.cpp
#sky.cpp
)

include(FindPkgConfig)
add_executable(${EXECUTABLE_NAME} ${SRC})

include_directories(
${CMAKE_SOURCE_DIR}
${SDL2_INCLUDE_DIR}
${SDLMIXER_INCLUDE_DIR}
${FREEIMAGE_INCLUDE_PATH}
${FREETYPE_INCLUDE_DIRS}
${GLM_INCLUDE_DIRS}
${FTGL_INCLUDE_DIR}
${EIGEN3_INCLUDE_DIR}
${YAMLCPP_INCLUDE_DIR}
${GLEW_INCLUDE_DIRS}
${LIBROCKET_INCLUDE_DIRS}
)

target_link_libraries(
${EXECUTABLE_NAME}
${SDL2_LIBRARY}
${SDL2MAIN_LIBRARY}
${SDLMIXER_LIBRARY}
${FREEIMAGE_LIBRARY}
${FREETYPE_LIBRARY}
${FTGL_LIBRARY}
${OPENGL_LIBRARIES}
${GLEW_LIBRARIES}
${LIBROCKET_LIBRARIES}
${LIBROCKET_DEBUGGER_LIBRARY}
${LIBROCKET_CONTROLS_LIBRARY}
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
install(DIRECTORY ./gui/ DESTINATION "gui") #FILES_MATCHING PATTERN "*")

install(DIRECTORY ./ DESTINATION bin FILES_MATCHING PATTERN "*.frag" )
install(DIRECTORY ./ DESTINATION bin FILES_MATCHING PATTERN "*.vert" )
install(TARGETS ${EXECUTABLE_NAME} DESTINATION bin)

