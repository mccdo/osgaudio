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

#include <osgAudio/Stream.h>
#include <osgAudio/BackendFMOD/AudioEnvironmentFMOD.h>
#include <osgAudio/SoundManager.h>

using namespace osgAudio;

Stream::Stream() throw (NameError) {
// nothing to do in plain-vanilla stream
} // Stream::Stream

Stream::Stream(const Stream &stream) {
	_FMODSound = NULL;
	createStreamFromFilename(stream.getFilename());
} // Stream::Stream

Stream &Stream::operator=(const Stream &stream) {
	if(this!=&stream) {
		// this doesn't seem to be exactly supported by FMOD
		// so we destroy any existing internal sample, and
		// create a new one patterned after the provided one
		if(_FMODSound) _FMODSound->release();
		_FMODSound = NULL;
		createStreamFromFilename(stream.getFilename());
	}
	return *this;
} // Stream::operator=

void Stream::createStreamFromFilename(const std::string& filename ) throw (FileError,NameError)
{
    _FMODSound = NULL;
    _filename = filename;

    FMOD_RESULT createResult;
    createResult = osgAudio::AudioEnvironment::instance()->
        getSystem()->createSound(filename.c_str(),
		FMOD_3D | osgAudio::SoundManager::instance()->getEnvironment()->getInternalDistanceModel(),
		0, &_FMODSound);

    if(createResult != FMOD_OK)
    {
        if(createResult == FMOD_ERR_FILE_BAD || createResult == FMOD_ERR_FILE_NOTFOUND)
        {
            throw NameError("Error loading file or File not found.");
        } // if
        else
        {
            throw FileError("Unknown error opening Stream file.");
        } // else
    } // if
} // Stream::createStreamFromFilename


Stream::~Stream() {
	// everything done in wrapped class
	if(_FMODSound) _FMODSound->release();
	_FMODSound = NULL;
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
// In FMOD, this is implemented in the Channel (aka, Source)
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

FMOD::Sound *Stream::getInternalSound(void) {
 return(_FMODSound);
} // Sample::getInternalSound

const FMOD::Sound *Stream::getInternalSound(void) const {
 return(_FMODSound);
} // Sample::getInternalSound
