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

#include <osgAudio/Stream.h>
#include <openalpp/Stream.h>

using namespace osgAudio;

Stream::Stream() throw (NameError) {
    try {
    _openalppStream = new openalpp::Stream ();
    }
    catch(openalpp::NameError error) { throw NameError(error.what()); }
} // Stream::Stream

Stream::Stream(const Stream &stream) {
    _openalppStream = new openalpp::Stream (*(stream.getInternalStream()));
} // Stream::Stream

Stream &Stream::operator=(const Stream &stream) {
    if(this!=&stream) {
        *_openalppStream = *(stream._openalppStream.get());
    }
    return *this;
} // Stream::operator=

void Stream::prepareForDeletion()
{
    _openalppStream->prepareForDeletion();
} // Stream::prepareForDeletion


Stream::~Stream() {
    // everything done in wrapped openalpp::Stream
} // Stream::~Stream

/*
// <<<>>> TBI
void Stream::record(ALuint sourcename) {
    if(!updater_)
        throw FatalError("No updater thread for stream!");
    if (!isRecording_)
        alSourcei(sourcename,AL_BUFFER,0);

    updater_->addSource(sourcename);
    isRecording_ = true;
} // Stream::record
*/


void Stream::seek(float time_s)
{
    _openalppStream->seek(time_s);
} // Stream::seek

/*
// <<<>>> TBI
void Stream::stop(ALuint sourcename) {
    if(!updater_)
        throw FatalError("No updater thread for stream!");
    updater_->removeSource(sourcename);
    isRecording_ = false;
} // Stream::stop
*/

openalpp::Stream *Stream::getInternalStream(void) {
 return(_openalppStream.get());
} // Stream::getInternalStream

const openalpp::Stream *Stream::getInternalStream(void) const {
 return(_openalppStream.get());
} // Stream::getInternalStream
