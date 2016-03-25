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
#include <openalpp/SoundData.h>

using namespace openalpp;

SoundData::SoundBuffer::SoundBuffer() throw (NameError) : buffername_(0) {
    alGenBuffers(1,&buffername_);

    if(alGetError()!=AL_FALSE)
    {
        throw NameError("Error generating buffer name");
    }
}

SoundData::SoundBuffer::~SoundBuffer() {
    alDeleteBuffers(1,&buffername_);
}


SoundData::SoundData() throw (NameError,InitError) : AudioBase() {
    buffer_=new SoundBuffer();
}



SoundData::SoundData(const SoundData &sounddata)
: AudioBase() {
    buffer_=sounddata.buffer_;
}

SoundData::~SoundData() 
{
    buffer_ = 0;
}

ALuint SoundData::getAlBuffer() const {
    return buffer_->getName();
}

SoundData &SoundData::operator=(const SoundData &sounddata) {
    if(this!=&sounddata) {
        buffer_=sounddata.buffer_;
    }
    return *this;
}

