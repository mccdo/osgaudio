# osgAudio CMake script
# modified Jan 2010 from osgAL CMake script by Sukender (Benoit Neil), from OSG CMake script

# TODO:
# Check on Mac OSX
# Copy includes in out-of-the-source builds ?
# Add documentation subproject ?

CMAKE_MINIMUM_REQUIRED(VERSION 2.6.2 FATAL_ERROR)

project( osgAudio )
SET(OSGAUDIO_MAJOR_VERSION 2)
SET(OSGAUDIO_MINOR_VERSION 0)
SET(OSGAUDIO_PATCH_VERSION 0)

SET(OSGAUDIO_VERSION ${OSGAUDIO_MAJOR_VERSION}.${OSGAUDIO_MINOR_VERSION}.${OSGAUDIO_PATCH_VERSION})

# We have some custom .cmake scripts not in the official distribution.
SET(CMAKE_MODULE_PATH "${osgAudio_SOURCE_DIR}/CMakeModules;${CMAKE_MODULE_PATH}")

# Note from Sukender (Benoit Neil): The "Findosg.cmake" in CMakeModules is somewhat buggy (but not removed); hence the version test.
IF(CMAKE_MAJOR_VERSION EQUAL 2 AND CMAKE_MINOR_VERSION EQUAL 6 AND CMAKE_PATCH_VERSION EQUAL 2)		# 2.6.2 works but don't have proper FindOpenSceneGraph module
	FIND_PACKAGE( osg262 REQUIRED )		# Uses custom finder. Useable in 2.6.2, but not as complete as the 2.6.3+ finders
ELSE()
	FIND_PACKAGE(OpenThreads)
	FIND_PACKAGE(OpenSceneGraph 2.8.3 REQUIRED COMPONENTS osg osgDB osgUtil osgGA osgText osgSim osgViewer)
	IF(OSG_FOUND)
		# Define shortcuts to conform to same variables names as 'osg262' finder
		SET(OSG_INCLUDE_DIRS ${OSG_INCLUDE_DIR} ${OSGDB_INCLUDE_DIR} ${OSGUTIL_INCLUDE_DIR} ${OSGGA_INCLUDE_DIR} ${OSGTEXT_INCLUDE_DIR} ${OSGSIM_INCLUDE_DIR} ${OSGVIEWER_INCLUDE_DIR} ${OPENTHREADS_INCLUDE_DIR})
		SET(OSG_LIBRARIES ${OSG_LIBRARIES} ${OSGDB_LIBRARIES} ${OSGUTIL_LIBRARIES} ${OSGGA_LIBRARIES} ${OSGTEXT_LIBRARIES} ${OSGSIM_LIBRARIES} ${OSGVIEWER_LIBRARIES} ${OPENTHREADS_LIBRARIES})
	ENDIF()
ENDIF()

SET(SUBSYSTEM_INCLUDES )
SET(SUBSYSTEM_LINKS )
SET(SUBSYSTEM_TARGET_LINKS )


# determine audio subsystem support
OPTION(0_ENABLE_SUBSYSTEM_OPENAL "Set to ON to build support for OpenAL/OpenAL++ audio subsystem." OFF)
OPTION(0_ENABLE_SUBSYSTEM_FMOD "Set to ON to build support for FMOD Ex audio subsystem." ON)

# OpenAL/OpenAL++ audio subsystem support
IF(0_ENABLE_SUBSYSTEM_OPENAL)
  SET(ALUT_USE_AL_SUBDIR ON)
  FIND_PACKAGE(ALUT)
  FIND_PACKAGE(OpenAL)
  FIND_PACKAGE(OGG)
  FIND_PACKAGE(Vorbis)
  FIND_PACKAGE(VorbisFile)
  #setup include and link Package names for this subsystem
  SET(SUBSYSTEM_OPENAL_INCLUDES ALUT OPENAL OGG VORBIS VORBISFILE)
  SET(SUBSYSTEM_OPENAL_LINKS ALUT OPENAL OGG VORBIS VORBISFILE)
  #append subsystem link/include package names to existing contents 
  SET(SUBSYSTEM_INCLUDES ${SUBSYSTEM_INCLUDES} ${SUBSYSTEM_OPENAL_INCLUDES})
  SET(SUBSYSTEM_LINKS  ${SUBSYSTEM_LINKS} ${SUBSYSTEM_OPENAL_LINKS})
  SET(SUBSYSTEM_TARGET_LINKS ${SUBSYSTEM_TARGET_LINKS} openalpp)
  # this is without the leading 0_ so we can use it as a C++ #define
  SET (ENABLE_SUBSYSTEM_OPENAL TRUE)
  IF(NOT ALUT_FOUND OR NOT OPENAL_FOUND OR NOT OGG_FOUND OR NOT VORBIS_FOUND OR NOT VORBISFILE_FOUND)
    MESSAGE("OpenAL and associated libraries not found. Consider using FMOD instead, using 0_ENABLE_SUBSYSTEM_FMOD option")
  ENDIF()
ENDIF(0_ENABLE_SUBSYSTEM_OPENAL)

# FMOD Ex support
IF(0_ENABLE_SUBSYSTEM_FMOD)
  FIND_PACKAGE(FMOD)
  #setup include and link Package names for this subsystem
  SET(SUBSYSTEM_FMOD_INCLUDES FMOD)
  SET(SUBSYSTEM_FMOD_LINKS FMOD)
  #append subsystem link/include package names to existing contents 
  SET(SUBSYSTEM_INCLUDES ${SUBSYSTEM_INCLUDES} ${SUBSYSTEM_FMOD_INCLUDES})
  SET(SUBSYSTEM_LINKS  ${SUBSYSTEM_LINKS} ${SUBSYSTEM_FMOD_LINKS})
  # I don't think this is needed
  # SET(SUBSYSTEM_TARGET_LINKS ${SUBSYSTEM_TARGET_LINKS} fmod)
  # this is without the leading 0_ so we can use it as a C++ #define
  SET (ENABLE_SUBSYSTEM_FMOD TRUE)
  IF(NOT FMOD_FOUND)
    MESSAGE("FMOD not found. Consider using OpenAL instead, using 0_ENABLE_SUBSYSTEM_OPENAL option")
  ENDIF()
ENDIF(0_ENABLE_SUBSYSTEM_FMOD)

# this is probably not the proper way to do this, twice.
configure_file (
  ${PROJECT_SOURCE_DIR}/include/osgAudio/Config.h.in
  ${PROJECT_SOURCE_DIR}/include/osgAudio/Config.h
  )
configure_file (
  ${PROJECT_SOURCE_DIR}/include/osgAudio/Config.h.in
  ${PROJECT_BINARY_DIR}/include/osgAudio/Config.h
  )

OPTION(0_BUILD_EXAMPLES_OSGAUDIO "Set to ON to build osgAudio examples." ON)
OPTION(0_BUILD_EXAMPLES_OSGAUDIO_LOWLEVEL "Set to ON to build osgAudio low-level examples." ON)
OPTION(0_BUILD_EXAMPLES_OALPP "Set to ON to build openAL++ examples." OFF)


# Make the headers visible to everything
IF(NOT ${PROJECT_BINARY_DIR} EQUAL ${PROJECT_SOURCE_DIR})
   INCLUDE_DIRECTORIES(${PROJECT_BINARY_DIR}/include)
ENDIF(NOT ${PROJECT_BINARY_DIR} EQUAL ${PROJECT_SOURCE_DIR})

INCLUDE_DIRECTORIES(
    ${osgAudio_SOURCE_DIR}/include
)


IF(WIN32)
    IF(MSVC)
        OPTION( WIN32_USE_MP "Build with multiple processes (/MP option; VS 2005 and above)." ON )
        MARK_AS_ADVANCED( WIN32_USE_MP )
        IF( WIN32_USE_MP )
            SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP" )
        ENDIF( WIN32_USE_MP )

        # # turn off various warnings
        #foreach(warning 4244 4251 4267 4275 4290 4786 4305 4996)
        foreach(warning 4290)
            SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd${warning}")
        endforeach(warning)

        # # More MSVC specific compilation flags
        ADD_DEFINITIONS(-D_SCL_SECURE_NO_WARNINGS)
        ADD_DEFINITIONS(-D_CRT_SECURE_NO_DEPRECATE)
    ENDIF(MSVC)
ENDIF(WIN32)


################################################################################
# Create bin and lib directories if required

SET(OUTPUT_BINDIR ${PROJECT_BINARY_DIR}/bin)
MAKE_DIRECTORY(${OUTPUT_BINDIR})
SET(OUTPUT_LIBDIR ${PROJECT_BINARY_DIR}/lib)
MAKE_DIRECTORY(${OUTPUT_LIBDIR})

SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_LIBDIR})
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_BINDIR})
IF(WIN32)
	SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_BINDIR})
ELSE(WIN32)
	SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_LIBDIR})
ENDIF(WIN32)

#MARK_AS_ADVANCED(...)

################################################################################
# Installation stuff

SET(CMAKE_DEBUG_POSTFIX "d" CACHE STRING "add a postfix, usually d on windows")
#ADD_DEFINITIONS(-DOSG_DEBUG_POSTFIX=${CMAKE_DEBUG_POSTFIX})

IF(UNIX AND NOT WIN32)
  IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
    ADD_DEFINITIONS("-D_DEBUG")
  ENDIF(CMAKE_BUILD_TYPE STREQUAL "Debug")
ENDIF(UNIX AND NOT WIN32)

IF(CYGWIN)
  IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
    ADD_DEFINITIONS("-D_DEBUG")
  ENDIF(CMAKE_BUILD_TYPE STREQUAL "Debug")
ENDIF(CYGWIN)


# Adds includes for a library found with FIND_PACKAGE
MACRO(INCLUDE_WITH_VARIABLES TRGTNAME)
    FOREACH(varname ${ARGN})
        IF(${varname}_INCLUDE_DIR)
			INCLUDE_DIRECTORIES("${${varname}_INCLUDE_DIR}")
        # ELSE(${varname}_INCLUDE_DIR)
			# MESSAGE(SEND_ERROR "Missing '${varname}_INCLUDE_DIR' variable")
        ENDIF(${varname}_INCLUDE_DIR)
    ENDFOREACH(varname)
ENDMACRO(INCLUDE_WITH_VARIABLES TRGTNAME)

# Adds includes and link properties for a library found with FIND_PACKAGE
FUNCTION(LINK_WITH_VARIABLES TRGTNAME)
	INCLUDE_WITH_VARIABLES(${TRGTNAME} ${ARGN})
	FOREACH(CUR_DEPENDENCY ${ARGN})
		TARGET_LINK_LIBRARIES(${TRGTNAME} ${${CUR_DEPENDENCY}_LIBRARIES} )
	ENDFOREACH()
ENDFUNCTION()
# MACRO(LINK_WITH_VARIABLES TRGTNAME)
	# INCLUDE_WITH_VARIABLES(${ARGV})
    # FOREACH(varname ${ARGN})
		# #MESSAGE(STATUS "${varname}_LIBRARY = ${${varname}_LIBRARY}")
        # IF(${varname}_LIBRARY_DEBUG)
			# #MESSAGE(STATUS "${varname}_LIBRARY_DEBUG = ${${varname}_LIBRARY_DEBUG}")
            # TARGET_LINK_LIBRARIES(${TRGTNAME} optimized "${${varname}_LIBRARY}" debug "${${varname}_LIBRARY_DEBUG}")
        # ELSE(${varname}_LIBRARY_DEBUG)
            # TARGET_LINK_LIBRARIES(${TRGTNAME} "${${varname}_LIBRARY}" )
        # ENDIF(${varname}_LIBRARY_DEBUG)
    # ENDFOREACH(varname)
# ENDMACRO(LINK_WITH_VARIABLES TRGTNAME)

# Slight hack to get the stock FindOpenAL.cmake script to work with
# our LINK_WITH_VARIABLES function. Function requires <foo>_LIBRARIES
# but stock FindOpenAL.cmake defines only OPENAL_LIBRARY.
SET( OPENAL_LIBRARIES ${OPENAL_LIBRARY} )

# macro for installing the example programs
MACRO( INSTALL_EXAMPLE EXENAME )
    INSTALL(
        TARGETS ${EXENAME}
        RUNTIME DESTINATION bin COMPONENT libosgaudio
    )
ENDMACRO( INSTALL_EXAMPLE EXENAME )



ADD_SUBDIRECTORY(src)

IF(0_BUILD_EXAMPLES_OSGAUDIO)
	ADD_SUBDIRECTORY(examples)
ENDIF(0_BUILD_EXAMPLES_OSGAUDIO)
