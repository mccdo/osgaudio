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

#include <openalpp/StreamUpdater.h>
#include <openalpp/Stream.h>

using namespace openalpp;

Stream::Stream() throw (NameError) : SoundData(), isRecording_(false) {
    buffer2_=new SoundData();
    updater_=NULL;
}

Stream::Stream(const Stream &stream) : SoundData((const SoundData &)stream) {
    buffer2_=stream.buffer2_;//->reference();
    updater_=stream.updater_;//->reference();
    isRecording_ = stream.isRecording_;
}

Stream &Stream::operator=(const Stream &stream) {
    if(this!=&stream) {
        SoundData::operator=((const SoundData &)stream);
        buffer2_=stream.buffer2_;//->reference();
        updater_=stream.updater_;//->reference();
    }
    return *this;
}

Stream::~Stream() {
    updater_ = 0L;
}

void Stream::record(ALuint sourcename) {
    if(!updater_)
        throw FatalError("No updater thread for stream!");
    if (!isRecording_)
        alSourcei(sourcename,AL_BUFFER,0);

    updater_->addSource(sourcename);
    isRecording_ = true;
}



void Stream::seek(float time_s)
{
    if (updater_.valid())
    {
        updater_->seek(time_s);
    }
}

void Stream::stop(ALuint sourcename) {
    if(!updater_)
        throw FatalError("No updater thread for stream!");
    updater_->removeSource(sourcename);
    isRecording_ = false;
}
