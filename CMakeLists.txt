cmake_minimum_required(VERSION 3.1)

project( SimpleSwitchBrowser )

set( CMAKE_CXX_STANDARD 17 )


list( APPEND CMAKE_MODULE_PATH
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake
)

include(utils)
include(options)
include(devkita64-libnx)

# to build NRO
include(nx-utils)

# submodules
include( borealisLib )


set( SUBMODULES_DIR ${PROJECT_SOURCE_DIR}/submodules )
include_directories( ${SUBMODULES_DIR}/cpp-generic-toolbox/include )

include_directories( ${SUBMODULES_DIR}/simple-cpp-logger/include )
add_definitions( -D LOGGER_MAX_LOG_LEVEL_PRINTED=6 )
add_definitions( -D LOGGER_PREFIX_LEVEL=3 )
add_definitions( -D LOGGER_ENABLE_COLORS_ON_USER_HEADER=1 )
add_definitions( -D LOGGER_TIME_FORMAT="\\\"%d/%m/%Y %H:%M:%S"\\\" )
add_definitions( -D LOGGER_PREFIX_FORMAT="\\\"{TIME} {USER_HEADER} {FILELINE}"\\\" )



# This project
add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/src )



