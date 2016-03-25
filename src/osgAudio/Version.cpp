/* -*-c++-*- */
/**
 * osgAudio - OpenSceneGraph Audio Library
 * (C) Copyright 2009-2012 byKenneth Mark Bryden
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


#include <osgAudio/Version.h>
#include <string>
#include <sstream>


unsigned int
osgAudio::getVersionNumber()
{
    return( OSGAUDIO_VERSION );
}


static std::string s_osgaudio_version( "" );

std::string
osgAudio::getVersion()
{
    if( s_osgaudio_version.empty() )
    {
        std::ostringstream oStr;
        oStr << std::string( "osgAudio version " ) <<
            OSGAUDIO_MAJOR_VERSION << "." <<
            OSGAUDIO_MINOR_VERSION << "." <<
            OSGAUDIO_SUB_VERSION << " (" <<
            getVersionNumber() << ").";
        s_osgaudio_version = oStr.str();
    }
    return( s_osgaudio_version );
}


std::string
osgAudio::getLibraryName()
{
    return "OpenSceneGraph Audio Library (osgAudio)";
}
