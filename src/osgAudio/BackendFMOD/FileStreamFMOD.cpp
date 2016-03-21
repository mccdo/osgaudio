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

#include <osgAudio/FileStream.h>


using namespace osgAudio;

FileStream::FileStream(const std::string& filename,const int buffersize)
throw (NameError,InitError,FileError) 
: Stream(0) {
	_FMODSound = NULL;

	createStreamFromFilename(filename); // on Stream class
} // FileStream::FileStream

FileStream::FileStream(const FileStream &stream)
: Stream(0) {
	_FMODSound = NULL;

	createStreamFromFilename(stream.getFilename());
} // FileStream::FileStream

FileStream::~FileStream() 
{ // nothing to do here, base Stream handles it
}

FileStream &FileStream::operator=(const FileStream &stream) {
	if(&stream!=this) 
	{
		// this doesn't seem to be exactly supported by FMOD
		// so we destroy any existing internal sample, and
		// create a new one patterend after the provided one
		if(_FMODSound) _FMODSound->release();
		_FMODSound = NULL;
		createStreamFromFilename(stream.getFilename());
	}
	return *this;
}

void FileStream::setLooping(bool loop) {
	// static cast to our derived subclass.
	// Cast could only fail if everything were amiss,
	// so dynamic_cast isn't an advantage here.
	if(_FMODSound)
	{
		/*
		Per FMOD documentation:

		Issues with streamed audio. (Sounds created with with System::createStream
		or FMOD_CREATESTREAM). When changing the loop count, sounds created with
		System::createStream or FMOD_CREATESTREAM may already have been pre-buffered
		and executed their loop logic ahead of time, before this call was even made.
		This is dependant on the size of the sound versus the size of the stream decode
		buffer. See FMOD_CREATESOUNDEXINFO.
		If this happens, you may need to reflush the stream buffer. To do this, you can
		call Channel::setPosition which forces a reflush of the stream buffer.
		Note this will usually only happen if you have sounds or looppoints that are
		smaller than the stream decode buffer size. Otherwise you will not normally
		encounter any problems.
		*/
		if(loop)
		{
			_FMODSound->setLoopCount(-1); // -1 = loop forever
			_FMODSound->setMode(FMOD_LOOP_NORMAL);
		} // if
		else
		{
			_FMODSound->setLoopCount(0);
			_FMODSound->setMode(FMOD_LOOP_OFF);
		} // else
	} // if
}

std::string FileStream::getFilename() const {
	char filenameBuffer[1024];
	filenameBuffer[0] = 0;
	if(_FMODSound) _FMODSound->getName(filenameBuffer, 1024);
	return filenameBuffer; // converts to std::string
}
