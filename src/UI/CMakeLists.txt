

set( LIB_NAME "UI" )
set( SRC_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/FrameMain.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/TabBrowser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/TabAbout.cpp
    )

add_library( ${LIB_NAME} STATIC ${SRC_FILES} )
install( TARGETS ${LIB_NAME} DESTINATION lib )

target_include_directories( ${LIB_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    )

target_link_libraries( ${LIB_NAME} PUBLIC
    Borealis
    switch::libnx
    -L/opt/devkitpro/portlibs/switch/lib
    -L/opt/devkitpro/libnx/lib
    -lglfw3 -lEGL -lglad -lglapi -ldrm_nouveau -lm -lnx
    )
