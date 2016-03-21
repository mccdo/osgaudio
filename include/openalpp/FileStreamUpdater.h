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

#ifndef OPENALPP_FILESTREAMUPDATER_H
#define OPENALPP_FILESTREAMUPDATER_H 1

#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

#include <openalpp/Export.h>
#include <openalpp/StreamUpdater.h>



namespace openalpp {

    /**
    * Updater for streams from files.
    */
    class OPENALPP_API FileStreamUpdater : public StreamUpdater 
    {
        OggVorbis_File *oggfile_; // The file structure
        const unsigned int buffersize_; // Size of the buffer in bytes
        ALshort *buffer_;
        bool seekPending_; // true if seek is waiting to be performed in update
        float seekTime_; // time in seconds to seek to
        bool looping_;            // Are we looping or not?

        /**
        * Perform seek on stream
        * @return true if seek was performed with no errors
        */
        bool seekNow(float time_s);

    public:
        /**
        * Constructor.
        * @param oggfile is the structure created by ov_open
        * @param buffer1 and...
        * @param buffer2 are the sound buffers to use.
        * @param format is the (OpenAL) format of the sound data.
        * @param frequency is the frequency of the sound data.
        * @param buffersize is the size of the buffer (in bytes)
        */
        FileStreamUpdater(OggVorbis_File *oggfile,
            const ALuint buffer1,ALuint buffer2,
            ALenum format,unsigned int frequency,
            unsigned int buffersize);


        /**
        * Inherited from Thread.
        * This will be called when the updater is Start():ed..
        */
        void run();

        /**
        * Seeks to specified time
        */
        void seek(float time_s);

        /**
        * Turn on/off looping.
        * @param loop is true if the stream should loop, false otherwise.
        */
        void setLooping(bool loop = true);

    protected:
        /**
        * Destructor.
        */
        virtual ~FileStreamUpdater();
    };

}

#endif // OPENALPP_FILESTREAMUPDATER_H
