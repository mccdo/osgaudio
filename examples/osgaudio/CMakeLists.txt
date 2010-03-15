SET(EXE_NAME example_osgaudio)

ADD_EXECUTABLE(
	${EXE_NAME}
	osgaudio.cpp
)

add_definitions( 
  -D_CONSOLE
  -DFL_DLL
)

INCLUDE_WITH_VARIABLES( ${EXE_NAME} ${SUBSYSTEM_INCLUDES} )
INCLUDE_DIRECTORIES( ${OSG_INCLUDE_DIRS} )
TARGET_LINK_LIBRARIES( ${EXE_NAME} ${OSG_LIBRARIES} )
TARGET_LINK_LIBRARIES( ${EXE_NAME} ${SUBSYSTEM_TARGET_LINKS} osgAudio )

# Add the postfix to the executable since it is not added automatically as for modules and shared libraries
SET_TARGET_PROPERTIES(${EXE_NAME} PROPERTIES DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}")
IF(MSVC_IDE)
	# Ugly workaround to remove the "/debug" or "/release" in each output
	SET_TARGET_PROPERTIES(${EXE_NAME} PROPERTIES PREFIX "../")
ENDIF()

INSTALL_EXAMPLE( ${EXE_NAME} )
