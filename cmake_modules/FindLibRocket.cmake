# - Try to find libRocket
# Once done this will define
#  LIBROCKET_FOUND - System has libRocket
#  LIBROCKET_INCLUDE_DIRS - The LibRocket include directories
#  LIBROCKET_LIBRARIES - The libraries needed to use LibRocket
#  LIBROCKET_DEFINITIONS - Compiler switches required for using LibRocket

find_path(LIBROCKET_INCLUDE_DIR Rocket/Core.h PATH_SUFFIXES Rocket )

find_library(LIBROCKET_LIBRARY NAMES RocketCore RocketCore_d)
find_library(LIBROCKET_DEBUGGER_LIBRARY NAMES RocketDebugger RocketDebugger_d)
find_library(LIBROCKET_CONTROLS_LIBRARY NAMES RocketControls RocketControls_d)

set(LIBROCKET_LIBRARY_DEBUGGER ${LIBROCKET_DEBUGGER_LIBRARY})
set(LIBROCKET_CONTROLS_LIBRARY ${LIBROCKET_CONTROLS_LIBRARY})

set(LIBROCKET_LIBRARIES ${LIBROCKET_LIBRARY} )
set(LIBROCKET_INCLUDE_DIRS ${LIBROCKET_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LIBROCKET_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(LibRocket DEFAULT_MSG LIBROCKET_LIBRARY LIBROCKET_INCLUDE_DIR)
find_package_handle_standard_args(LibRocket DEFAULT_MSG LIBROCKET_DEBUGGER_LIBRARY LIBROCKET_INCLUDE_DIR)
find_package_handle_standard_args(LibRocket DEFAULT_MSG LIBROCKET_CONTROLS_LIBRARY LIBROCKET_INCLUDE_DIR)

mark_as_advanced(LIBROCKET_INCLUDE_DIR LIBROCKET_LIBRARY )
mark_as_advanced(LIBROCKET_INCLUDE_DIR LIBROCKET_DEBUGGER_LIBRARY )
mark_as_advanced(LIBROCKET_INCLUDE_DIR LIBROCKET_CONTROLS_LIBRARY )
