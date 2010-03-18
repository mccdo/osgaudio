/* -*-c++-*- */
/**
 * osgAudio - OpenSceneGraph Audio Library
 * Copyright (C) 2010 AlphaPixel, LLC
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
 */

#include <osgAudio/Sample.h>
#include <osgAudio/AudioEnvironment.h>

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
	createResult = AudioEnvironment::getSystem()->createSound(filename.c_str(), FMOD_3D, 0, &_FMODSound);

	if(createResult != FMOD_OK)
	{
		if(createResult == FMOD_ERR_FILE_BAD || createResult == FMOD_ERR_FILE_NOTFOUND)
		{
			throw NameError("Error loading file or File not found.");
		} // if
		else
		{
			throw FileError("Unknown error opening Sample file.");
		} // else
	} // if
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
	char filenameBuffer[1024];
	filenameBuffer[0] = 0;
	if(_FMODSound) _FMODSound->getName(filenameBuffer, 1024);
	return filenameBuffer; // converts to std::string
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
