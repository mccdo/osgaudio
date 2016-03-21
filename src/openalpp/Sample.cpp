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

#include <sstream>

#include <openalpp/Sample.h>
#include <openalpp/windowsstuff.h>

#include <AL/alut.h>

using namespace openalpp;

Sample::Sample(const std::string& filename) throw (FileError)
: SoundData(),filename_(filename) {
#if OPENAL_VERSION < 2007
    ALsizei size;

    ALsizei freq;
#  if OPENAL_VERSION < 2005
    ALsizei bits;
    ALboolean success;
#  endif // OPENAL_VERSION < 2005
    ALenum format,error;
    ALvoid* data;
    ALboolean loop;

#  if OPENAL_VERSION < 2005
    success=alutLoadWAV(filename.c_str(),&data,&format,&size,&bits,&freq);
    if(success==AL_TRUE && data ) {
#  else // OPENAL_VERSION < 2005
    alutLoadWAVFile(const_cast<ALbyte *>(filename.c_str()),&format,&data,&size,&freq,&loop);
    if(data) {
#  endif // OPENAL_VERSION < 2005
        alBufferData(buffer_->buffername_,format,data,size,freq);
        if((error=alGetError())!=AL_FALSE)
            throw FileError("Error buffering sound");
        free(data);
    } else {
        std::ostringstream str;
        str << "Unable to load file: " << filename << std::ends;
        throw FileError(str.str().c_str());
    }
#else // OPENAL_VERSION < 2007
    ALuint success = alutCreateBufferFromFile (filename.c_str());

    if(success!=AL_NONE) {
        buffer_ = new SoundBuffer(success);

    } else {
        ALenum error = alutGetError ();
        const char *error_str = alutGetErrorString (error);
        std::ostringstream str;
        str << "Error loading file: " << filename << ": " << error_str << std::ends;
        throw FileError(str.str().c_str());
    }
#endif // OPENAL_VERSION < 2007
}

Sample::Sample(const Sample &sample)
: SoundData(sample), filename_(sample.filename_) {
}

Sample::Sample(ALenum format,ALvoid* data,ALsizei size,ALsizei freq) throw (FileError)
: SoundData() {
    ALenum error;

    alBufferData(buffer_->getName(),format,data,size,freq);
    if((error=alGetError())!=AL_FALSE)
        throw FileError("Error buffering sound");
}

std::string Sample::getFileName() const {
    return filename_;
}


Sample::~Sample()
{

}

Sample &Sample::operator=(const Sample &sample) {
    if(this!=&sample) {
        SoundData::operator=(sample);
        filename_=sample.filename_;
    }
    return *this;
}

// A couple of utility functions. Might move them to their own file later...
unsigned int openalpp::sampleSize(SampleFormat format) {
    switch(format) {
    case(Mono8):
        return 1;
        break;
    case(Stereo8):
    case(Mono16):
        return 2;
        break;
    case(Stereo16):
        return 4;
        break;
    default:
        return 0;
    }
}

unsigned int openalpp::sampleSize(ALenum format) {
    switch(format) {
    case(AL_FORMAT_MONO8):
        return 1;
        break;
    case(AL_FORMAT_STEREO8):
    case(AL_FORMAT_MONO16):
        return 2;
        break;
    case(AL_FORMAT_STEREO16):
        return 4;
        break;
    default:
        return 0;
    }
}

