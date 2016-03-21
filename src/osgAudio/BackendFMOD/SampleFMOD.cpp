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

#include <osgAudio/Sample.h>
#include <osgAudio/AudioEnvironment.h>
#include <osgAudio/SoundManager.h>

using namespace osgAudio;


Sample::Sample(const std::string& filename) throw (FileError,NameError) {
	_FMODSound = NULL;

	createSampleFromFilename(filename);

} // Sample::Sample


Sample::Sample(const Sample &sample) {
	_FMODSound = NULL;

	createSampleFromFilename(sample.getFilename());
} // Sample::Sample


void Sample::createSampleFromFilename(const std::string& filename ) throw (FileError,NameError)
 {
	_FMODSound = NULL;

	FMOD_RESULT createResult;
	createResult = osgAudio::AudioEnvironment::instance()->getSystem()->
     createSound(filename.c_str(),
	 FMOD_3D | osgAudio::AudioEnvironment::instance()->getInternalDistanceModel(),
	 0, &_FMODSound);

	if(createResult != FMOD_OK)
	{
		if(createResult == FMOD_ERR_FILE_BAD || createResult == FMOD_ERR_FILE_NOTFOUND)
		{
            std::string exep = "Error loading file or file not found " + filename;
			throw NameError( exep );
		} // if
		else
		{
			throw FileError("Unknown error opening Sample file.");
		} // else
	} // if
	_internalFullPath = filename;
} // Sample::createSampleFromFilename


// <<<>>> TODO: Create a portable way of specifying the format
/*
Sample::Sample(ALenum format,ALvoid* data,ALsizei size,ALsizei freq) throw (FileError)
: SoundData() {
	_FMODSound = NULL;

//		throw FileError("Error buffering sound");
}
*/

std::string Sample::getFilename() const {
	return _internalFullPath;
}


Sample::~Sample()
{
	if(_FMODSound) _FMODSound->release();
	_FMODSound = NULL;
}

Sample &Sample::operator=(const Sample &sample) {
	if(this!=&sample) {
		// this doesn't seem to be exactly supported by FMOD
		// so we destroy any existing internal sample, and
		// create a new one patterend after the provided one
		if(_FMODSound) _FMODSound->release();
		_FMODSound = NULL;
		createSampleFromFilename(sample.getFilename());
	}
	return *this;
}

FMOD::Sound *Sample::getInternalSound(void) {
 return(_FMODSound);
} // Sample::getInternalSound

const FMOD::Sound *Sample::getInternalSound(void) const {
 return(_FMODSound);
} // Sample::getInternalSound
