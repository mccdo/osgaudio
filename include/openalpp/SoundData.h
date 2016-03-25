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

#ifndef OPENALPP_SOUNDDATA_H
#define OPENALPP_SOUNDDATA_H 1

#include <osg/Referenced>
#include <osg/ref_ptr>

#include <openalpp/Export.h>
#include <openalpp/Error.h>
#include <openalpp/AudioBase.h>

#ifdef WIN32
// Ignore the dll interface warning using std::vector members
#pragma warning(disable : 4251)
#endif


namespace openalpp {

    /**
    * Base class for sound data.
    */
    class OPENALPP_API SoundData : public AudioBase {
    protected:
        /**
        * Protected class to handle generation/deletion of OpenAL buffers correctly.
        */
        class SoundBuffer : public osg::Referenced {
            ALuint buffername_;
        protected:
            virtual ~SoundBuffer();

        public:
            SoundBuffer() throw (NameError);
            SoundBuffer(ALuint buffer_id) throw (NameError) { buffername_ = buffer_id; }
            ALuint getName() {return buffername_;}
        };
    public:
        /**
        * Get the OpenAL name for the buffer.
        * @return the OpenAL name.
        */
        ALuint getAlBuffer() const;

        /**
        * Constructor.
        */
        SoundData() throw (NameError,InitError);



        /**
        * Copy constructor.
        */
        SoundData(const SoundData &sounddata);

        /**
        * Assignment operator.
        */
        SoundData &operator=(const SoundData &sounddata);

    protected:

        /**
        * Destructor.
        */
        virtual ~SoundData();

        /**
        * See class SoundBuffer comment.
        */
        osg::ref_ptr<SoundBuffer> buffer_;

        /**
        * OpenAL name for the buffer.
        */
        //ALuint buffername_;
    };

}

#endif /* OPENALPP_SOUNDDATA_H */
