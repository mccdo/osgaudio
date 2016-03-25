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

#include <osgAudio/Sample.h>

#include <openalpp/Sample.h>

using namespace osgAudio;


Sample::Sample(const std::string& filename) throw (FileError,NameError) {
    try {
    _openalppSample = new openalpp::Sample (filename);
    }
    catch(openalpp::NameError error) { throw NameError(error.what()); }
    catch(openalpp::FileError error) { throw FileError(error.what()); }
}

Sample::Sample(const Sample &sample) {
    _openalppSample = new openalpp::Sample (*(sample.getInternalSample()));
}

// <<<>>> TODO: Create a portable way of specifying the format
/*
Sample::Sample(ALenum format,ALvoid* data,ALsizei size,ALsizei freq) throw (FileError)
: SoundData() {
//        throw FileError("Error buffering sound");
}
*/

std::string Sample::getFilename() const {
    return _openalppSample->getFileName();
}


Sample::~Sample()
{
}

Sample &Sample::operator=(const Sample &sample) {
    if(this!=&sample) {
        *_openalppSample = *(sample._openalppSample.get());
    }
    return *this;
}

openalpp::Sample *Sample::getInternalSample(void) {
 return(_openalppSample.get());
}

const openalpp::Sample *Sample::getInternalSample(void) const {
 return(_openalppSample.get());
}
