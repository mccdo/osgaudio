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

#ifndef OSGAUDIO_SOUNDFMOD_H
#define OSGAUDIO_SOUNDFMOD_H 1

#include <osgAudio/Export.h>
#include <osgAudio/Error.h>

#include <fmod.hpp>


namespace osgAudio 
{

	/// A Sound is a generic interface to a Sample or Stream
	/*!
	Mostly this is just used to store a ref-counted pointer to another object.
	*/

	class OSGAUDIO_EXPORT Sound : public osg::Referenced  {
	public:
		/**
		* Get internal FMOD object (used only by osgAudio FMOD layer)
		* @return FMOD::Sound object wrapped by osgAudio.
		* Must be over-ridden in subclasses such as Sample, FileStream
		*/
		virtual FMOD::Sound *getInternalSound(void) {return(NULL);};
		virtual const FMOD::Sound *getInternalSound(void) const {return(NULL);};

	protected:
		virtual ~Sound() {};
		}; // Sound

} // namespace osgAudio

#endif /* OSGAUDIO_SOUNDFMOD_H */
