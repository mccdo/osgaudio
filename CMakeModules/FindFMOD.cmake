# Locate FMOD
# This module defines FMOD_FOUND, FMOD_INCLUDE_DIR and FMOD_LIBRARIES standard variables

SET( FMOD_SEARCH_PATHS
	~/Library/Frameworks
	/Library/Frameworks
	/usr/local
	/usr
	/sw # Fink
	/opt/local # DarwinPorts
	/opt/csw # Blastwave
	/opt
)

FIND_PATH( FMOD_INCLUDE_DIR
	NAMES fmod.h
	HINTS
	$ENV{FMOD_DIR}
	${FMOD_DIR}
	PATH_SUFFIXES inc include
	PATHS ${FMOD_SEARCH_PATHS}
)

FIND_LIBRARY( FMOD_LIBRARY 
	NAMES fmod fmodvc libfmod fmodex_vc
	HINTS
	$ENV{FMOD_DIR}
	${FMOD_DIR}
	PATH_SUFFIXES lib lib64
	PATHS ${FMOD_SEARCH_PATHS}
)

IF( FMOD_LIBRARY )
    SET( FMOD_LIBRARIES "${FMOD_LIBRARY}")		# Could add "general" keyword, but it is optional
ENDIF()

# handle the QUIETLY and REQUIRED arguments and set FMOD_FOUND to TRUE if all listed variables are TRUE
INCLUDE( FindPackageHandleStandardArgs )
FIND_PACKAGE_HANDLE_STANDARD_ARGS( FMOD DEFAULT_MSG FMOD_LIBRARIES FMOD_INCLUDE_DIR )
