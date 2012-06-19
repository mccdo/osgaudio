# osgAudio CMake script
# modified Jan 2010 from osgAL CMake script by Sukender (Benoit Neil), from OSG CMake script

# TODO:
# Copy includes in out-of-the-source builds ?

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8 FATAL_ERROR)

project( osgAudio )
# Note: These values are used by include/osgwTools/Version.in
# to generate the Version.h file.
SET(OSGAUDIO_MAJOR_VERSION 2)
SET(OSGAUDIO_MINOR_VERSION 0)
SET(OSGAUDIO_SUB_VERSION 0)

SET(OSGAUDIO_VERSION ${OSGAUDIO_MAJOR_VERSION}.${OSGAUDIO_MINOR_VERSION}.${OSGAUDIO_SUB_VERSION})

# We have some custom .cmake scripts not in the official distribution.
SET(CMAKE_MODULE_PATH "${osgAudio_SOURCE_DIR}/CMakeModules;${CMAKE_MODULE_PATH}")

include( FindOSGHelper )


SET(SUBSYSTEM_INCLUDES )
SET(SUBSYSTEM_LINKS )
SET(SUBSYSTEM_TARGET_LINKS )


# determine audio subsystem support
OPTION(0_ENABLE_SUBSYSTEM_OPENAL "Set to ON to build support for OpenAL/OpenAL++ audio subsystem." OFF)
OPTION(0_ENABLE_SUBSYSTEM_FMOD "Set to ON to build support for FMOD Ex audio subsystem." ON)

SET(ENABLE_SUBSYSTEM_FMOD FALSE)
SET(ENABLE_SUBSYSTEM_OPENAL FALSE)

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

option( OSGAUDIO_INSTALL_DATA "Enable to add the data directory to the install target" ON )
if( OSGAUDIO_INSTALL_DATA )
    add_subdirectory( data )
endif()


ADD_SUBDIRECTORY(src)

IF(0_BUILD_EXAMPLES_OSGAUDIO)
    ADD_SUBDIRECTORY(examples)
ENDIF(0_BUILD_EXAMPLES_OSGAUDIO)


#
# Doxygen
find_package( Doxygen )
if( DOXYGEN_FOUND )
    set( HAVE_DOT "NO" )
    if( DOXYGEN_DOT_PATH )
        set( HAVE_DOT "YES" )
    endif()
    
    configure_file( ${PROJECT_SOURCE_DIR}/doc/doxyfile.cmake
        ${PROJECT_BINARY_DIR}/doc/doxyfile
    )
    add_custom_target( Documentation ${DOXYGEN_EXECUTABLE}
        ${PROJECT_BINARY_DIR}/doc/doxyfile
    )

    add_subdirectory( doc )
endif()

# On windows provide the user with the pdb files 
# for debugging if they are present
if(MSVC)
    install(
        DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/\${CMAKE_INSTALL_CONFIG_NAME}/
        DESTINATION lib
        USE_SOURCE_PERMISSIONS
        COMPONENT libosgaudio-dev
        FILES_MATCHING PATTERN "*.pdb"
    )
endif(MSVC)

# Install the CMake find script for osgWorks
install( FILES "extra/FindosgAudio.cmake"
    DESTINATION "share/${CMAKE_PROJECT_NAME}/extra"
    COMPONENT libosgaudio
)

#
# Create and install all of the auto find tools
# =============================================

# Add all targets to the build-tree export set
IF(0_ENABLE_SUBSYSTEM_OPENAL)
    export(TARGETS osgAudio openalpp
        FILE "${PROJECT_BINARY_DIR}/lib/osgAudioLibraryDepends.cmake")
ELSE(0_ENABLE_SUBSYSTEM_OPENAL)
    export(TARGETS osgAudio
        FILE "${PROJECT_BINARY_DIR}/lib/osgAudioLibraryDepends.cmake")
ENDIF(0_ENABLE_SUBSYSTEM_OPENAL)

# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
export(PACKAGE osgAudio)

# Create a osgWorksBuildTreeSettings.cmake file for the use from the build tree
configure_file(osgAudioBuildTreeSettings.cmake.in
  "${PROJECT_BINARY_DIR}/lib/osgAudioBuildTreeSettings.cmake" @ONLY)

#configure_file(osgAudioConfig.cmake.in
#  "${PROJECT_BINARY_DIR}/lib/osgAudioConfig.cmake" @ONLY)
configure_file(osgAudioConfigVersion.cmake.in
  "${PROJECT_BINARY_DIR}/lib/osgAudioConfigVersion.cmake" @ONLY)
configure_file(UseosgAudio.cmake.in
  "${PROJECT_BINARY_DIR}/lib/UseosgAudio.cmake" @ONLY)

# Install the export set for use with the install-tree
install(EXPORT osgaudio-targets DESTINATION lib)

include( CMakePackageConfigHelpers )
# Create the osgAudioConfig.cmake and osgAudioConfigVersion files
#file(RELATIVE_PATH CONF_REL_INCLUDE_DIR "${CMAKE_INSTALL_PREFIX}/lib"
#   "${CMAKE_INSTALL_PREFIX}/include")

configure_package_config_file( 
    osgAudioConfig.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/lib/osgAudioConfig.cmake
    INSTALL_DESTINATION lib
    PATH_VARS ENABLE_SUBSYSTEM_OPENAL ENABLE_SUBSYSTEM_FMOD )

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/lib/osgAudioConfig.cmake 
              ${CMAKE_CURRENT_BINARY_DIR}/lib/osgAudioConfigVersion.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/lib/UseosgAudio.cmake
        DESTINATION lib )
