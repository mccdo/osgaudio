# Locate ALUT
# This module defines
# ALUT_LIBRARY
# ALUT_FOUND, if false, do not try to link to ALUT 
# ALUT_INCLUDE_DIR, where to find the headers
#
# $ALUTDIR is an environment variable that would
# correspond to the ./configure --prefix=$ALUTDIR
# used in building ALUT.
#
# Created by Sukender (Benoit Neil). Based on FindOpenAL.cmake module.

FIND_PATH(ALUT_INCLUDE_DIR alut.h
	HINTS
	$ENV{ALUTDIR}
	$ENV{ALUT_PATH}
	PATH_SUFFIXES include/AL include
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

FIND_LIBRARY(ALUT_LIBRARY 
	alut
	HINTS
	$ENV{ALUTDIR}
	$ENV{ALUT_PATH}
	PATH_SUFFIXES lib64 lib libs64 libs libs/Win32 libs/Win64
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

FIND_LIBRARY(ALUT_LIBRARY_DEBUG 
	alutd
	HINTS
	$ENV{ALUTDIR}
	$ENV{ALUT_PATH}
	PATH_SUFFIXES lib64 lib libs64 libs libs/Win32 libs/Win64
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

SET(ALUT_FOUND "NO")
IF(ALUT_LIBRARY AND ALUT_INCLUDE_DIR)
	SET(ALUT_FOUND "YES")
ENDIF(ALUT_LIBRARY AND ALUT_INCLUDE_DIR)
