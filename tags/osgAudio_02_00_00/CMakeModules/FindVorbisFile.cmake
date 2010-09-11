# Locate VorbisFile
# This module defines
# VORBISFILE_LIBRARY
# VORBISFILE_FOUND, if false, do not try to link to VorbisFile
# VORBISFILE_INCLUDE_DIR, where to find the headers
#
# $VORBISDIR is an environment variable that would
# correspond to the ./configure --prefix=$VORBISDIR
# used in building Vorbis.
#
# Created by Sukender (Benoit Neil). Based on FindOpenAL.cmake module.
# TODO Add hints for linux and Mac

FIND_PATH(VORBISFILE_INCLUDE_DIR
	vorbis/vorbisfile.h
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

FIND_LIBRARY(VORBISFILE_LIBRARY 
	NAMES vorbisfile
	HINTS
	$ENV{VORBISDIR}
	$ENV{VORBIS_PATH}
	PATH_SUFFIXES win32/VorbisFile_Dynamic_Release
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

FIND_LIBRARY(VORBISFILE_LIBRARY_DEBUG 
	NAMES vorbisfile_d
	HINTS
	$ENV{VORBISDIR}
	$ENV{VORBIS_PATH}
	PATH_SUFFIXES win32/VorbisFile_Dynamic_Debug
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

