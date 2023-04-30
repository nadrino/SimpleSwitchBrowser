cmake_minimum_required(VERSION 3.1)

project( SimpleSwitchBrowser )

list( APPEND CMAKE_MODULE_PATH
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake
)

include(utils)
include(options)
include(devkita64-libnx)


set( CMAKE_CXX_STANDARD 17 )




