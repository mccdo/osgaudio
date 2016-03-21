/* -*-c++-*- */
/**
 * osgAudio - OpenSceneGraph Audio Library
 * (C) Copyright 2009-2012 by Kenneth Mark Bryden
 * (programming by Chris 'Xenon' Hanson, AlphaPixel, LLC xenon at alphapixel.com)
 * based on a fork of:
 * Osg AL - OpenSceneGraph Audio Library
 * Copyright (C) 2004 VRlab, Umeå University
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * Please see COPYING file for special static-link exemption to LGPL.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef OPENALPP_WINDOWSSTUFF_H
#define OPENALPP_WINDOWSSTUFF_H 1

/**
  Due to a lack of version numbering in OpenAL headers
  the macro OPENAL_VERSION was introduced to separate between
  different versions.
  This will make OpenAL++ compatible with numerous OpenAL versions.
*/
//#define OPENAL_VERSION 2004
//#define OPENAL_VERSION 2005
//#define OPENAL_VERSION 2006
#define OPENAL_VERSION 2007


#ifndef WIN32
  
    #include <unistd.h>

#else // WIN32

    #ifdef _MSC_VER
        // C++ exception specification ignored except to indicate a function is not __declspec(nothrow)
        //#pragma warning( disable : 4290 )
    #endif

    #include <openalpp/Export.h>
    #include <windows.h>

    inline void usleep( int x ) { Sleep( x /1000 ); };

    #include <AL/alut.h>

    #define alGetSourceiv alGetSourcei

#endif // WIN32

#endif    // OPENALPP_WINDOWSSTUFF_H
