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

#ifndef OPENALPP_STREAM_H
#define OPENALPP_STREAM_H 1

#include <osg/ref_ptr>

#include <openalpp/Export.h>
#include <openalpp/SoundData.h>
#include <openalpp/StreamUpdater.h>

#ifdef WIN32
// Ignore the dll interface warning using std::vector members
#pragma warning(disable : 4251)
#endif


namespace openalpp {


    /**
    * Base class for NetStream and InputDevice.
    * Used for audio streams.
    */
    class OPENALPP_API Stream : public SoundData {
    protected:
        /**
        * For double-buffering of sounds.
        */
        osg::ref_ptr<SoundData> buffer2_;

        osg::ref_ptr<StreamUpdater> updater_;
    public:
        /**
        * Default constructor.
        */
        Stream() throw (NameError);

        /**
        * Copy constructor.
        */
        Stream(const Stream &stream);

        /**
        * Assignment operator.
        */
        Stream &operator=(const Stream &stream);


        void prepareForDeletion() {updater_->cancel();}

        /**
        * Start recording.
        * I.e. start copying data to buffers.
        * @param sourcename is the (OpenAL) name of the source.
        */
        void record(ALuint sourcename);

        /**
        * Seeks to specified time
        */
        void seek(float time_s); 

        /**
        * Stop recording.
        * @param sourcename is the (OpenAL) name of the source.
        */
        void stop(ALuint sourcename);

    protected:
        /**
        * Destructor.
        */
        virtual ~Stream();

        bool isRecording_;
    };

}

#endif /* OPENALPP_STREAM_H */
