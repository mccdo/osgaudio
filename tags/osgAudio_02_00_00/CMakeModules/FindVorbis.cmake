# Locate Vorbis
# This module defines
# VORBIS_LIBRARY
# VORBIS_FOUND, if false, do not try to link to Vorbis 
# VORBIS_INCLUDE_DIR, where to find the headers
#
# $VORBISDIR is an environment variable that would
# correspond to the ./configure --prefix=$VORBISDIR
# used in building Vorbis.
#
# Created by Sukender (Benoit Neil). Based on FindOpenAL.cmake module.
# TODO Add hints for linux and Mac

FIND_PATH(VORBIS_INCLUDE_DIR
	vorbis/codec.h
	HINTS
	$ENV{VORBISDIR}
	$ENV{VORBIS_PATH}
	PATH_SUFFIXES include
	PATHS
	~/Library/Frameworks
	/Library/Frameworks
	/usr/local
	/usr
	/sw # Fink
	/opt/local # DarwinPorts
	/opt/csw # Blastwave
	/opt
)

FIND_LIBRARY(VORBIS_LIBRARY 
	vorbis
	HINTS
	$ENV{VORBISDIR}
	$ENV{VORBIS_PATH}
	PATH_SUFFIXES win32/Vorbis_Dynamic_Release
	PATHS
	~/Library/Frameworks
	/Library/Frameworks
	/usr/local
	/usr
	/sw
	/opt/local
	/opt/csw
	/opt
)

FIND_LIBRARY(VORBIS_LIBRARY_DEBUG 
	vorbis_d
	HINTS
	$ENV{VORBISDIR}
	$ENV{VORBIS_PATH}
	PATH_SUFFIXES win32/Vorbis_Dynamic_Debug
	PATHS
	~/Library/Frameworks
	/Library/Frameworks
	/usr/local
	/usr
	/sw
	/opt/local
	/opt/csw
	/opt
)


SET(VORBIS_FOUND "NO")
IF(VORBIS_LIBRARY AND VORBIS_INCLUDE_DIR)
	SET(VORBIS_FOUND "YES")
ENDIF(VORBIS_LIBRARY AND VORBIS_INCLUDE_DIR)

