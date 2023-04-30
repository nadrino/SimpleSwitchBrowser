


# Replace this with the name of your application
set( GUI_NAME "SimpleSwitchBrowser")
set( GUI_APP "${GUI_NAME}")

# Meta information about the app
set( GUI_TITLE ${GUI_NAME})
set( GUI_AUTHOR "Nadrino")
set( GUI_ICON  "${CMAKE_CURRENT_SOURCE_DIR}/resources/assets/icon_gui.jpg")
set( GUI_ROMFS "${CMAKE_CURRENT_SOURCE_DIR}/resources/romfs")

set( SRC_FILES src/SimpleSwitchBrowser.cxx )



add_executable( ${GUI_APP}.elf ${SRC_FILES} )
target_include_directories( ${GUI_APP}.elf PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include )

target_link_libraries(
    ${GUI_APP}.elf PUBLIC
    Borealis
    switch::libnx
    -L/opt/devkitpro/portlibs/switch/lib
    -L/opt/devkitpro/libnx/lib
    ${ZLIB_LIBRARIES}
    ${FREETYPE_LIBRARIES}
    -lglfw3 -lEGL -lglad -lglapi -ldrm_nouveau -lm -lnx
)

set_target_properties(${GUI_APP}.elf PROPERTIES
    LINKER_LANGUAGE CXX # Replace this with C if you have C source files
    LINK_FLAGS "-specs=${LIBNX}/switch.specs -Wl,-no-as-needed -Wl,-Map,.map"
    )


set(CMAKE_BUILD_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${GUI_APP}.elf.dir)
set(BUILD_ROMFS ${CMAKE_BUILD_DIRECTORY}/Resources)
cmake_info("Resources will be gathered in: ${BUILD_ROMFS}")
add_custom_command(
    TARGET ${GUI_APP}.elf
    PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Gathering resources..."
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${BUILD_ROMFS}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${BUILD_ROMFS}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${BUILD_ROMFS}/borealis
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${GUI_ROMFS}/. ${BUILD_ROMFS}/.
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${BOREALIS_DIR}/resources/. ${BUILD_ROMFS}/borealis/.
    COMMAND ${CMAKE_COMMAND} -E echo "Resources have been gathered."
)


cmake_info( "GUI_APP = ${GUI_APP}" )
cmake_info( "GUI_TITLE = ${GUI_TITLE}" )
cmake_info( "GUI_AUTHOR = ${GUI_AUTHOR}" )
cmake_info( "GUI_ICON = ${GUI_ICON}" )
cmake_info( "BUILD_ROMFS = ${BUILD_ROMFS}" )

build_switch_binaries(
    ${GUI_APP}.elf
    ${GUI_TITLE} ${GUI_AUTHOR} ${GUI_ICON} "0.0.1" ${BUILD_ROMFS}
)
