cmake_minimum_required(VERSION 3.5)

project( SimpleSwitchBrowser )

set( CMAKE_CXX_STANDARD 17 )


list( APPEND CMAKE_MODULE_PATH
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake
)

set( CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/cmake/devkita64-libnx.cmake )

include(utils)
include(options)
include(devkita64-libnx)

# Git version
execute_process(
    COMMAND git describe --tags
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE VERSION
    RESULT_VARIABLE RETURN_VAL
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

cmake_info("Git version: ${VERSION}")
if( RETURN_VAL EQUAL "0" )

  string(REPLACE "-" ";" VERSION_SEP ${VERSION})
  list(LENGTH VERSION_SEP len)

  if( ${len} GREATER_EQUAL 2 )
    list(GET VERSION_SEP 0 VERSION_STR)         # VERSION_SEP[0] = X.X.X
    list(GET VERSION_SEP 1 VERSION_POST_NB)     # VERSION_SEP[1] = NB COMMITS AFTER TAG
    list(GET VERSION_SEP 2 VERSION_POST_COMMIT) # VERSION_COMMIT[2] = "g" + COMMIT HASH
    #    set(VERSION_TAG "-${VERSION_POST_NB}-${VERSION_POST_COMMIT}")
    set(VERSION_TAG "f+${VERSION_POST_NB}")
  else()
    list(GET VERSION_SEP 0 VERSION_STR)         # VERSION_SEP[0] = X.X.X
    set(VERSION_TAG "")
  endif()

  # Parsing version number
  string(REPLACE "." ";" VERSION_STR_SEP ${VERSION_STR})
  list(GET VERSION_STR_SEP 0 VERSION_MAJOR)
  list(GET VERSION_STR_SEP 1 VERSION_MINOR)
  list(GET VERSION_STR_SEP 2 VERSION_REVISION)

  set (VERSION_STRING "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_REVISION}${VERSION_TAG}")
  set (VERSION_SHORT_STRING "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_REVISION}")
else()
  cmake_info("Bad exit status")
  set (VERSION_STRING "X.X.X")
endif()
cmake_info( "Identified Version ${VERSION_STRING}" )

configure_file( ${CMAKE_SOURCE_DIR}/cmake/VersionConfig.h.in ${CMAKE_BINARY_DIR}/generated/VersionConfig.h )
include_directories( ${CMAKE_BINARY_DIR}/generated/ )





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



