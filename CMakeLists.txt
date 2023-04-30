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


# This project
add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/src )



