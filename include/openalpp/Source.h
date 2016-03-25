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

#ifndef OPENALPP_SOURCE_H
#define OPENALPP_SOURCE_H 1

#include <openalpp/Export.h>
#include <openalpp/Stream.h>
#include <openalpp/FileStream.h>
#include <openalpp/SourceBase.h>
#include <openalpp/Sample.h>

namespace openalpp {

    /**
    * Class for "normal" sources.
    * This is used for standard OpenAL sources, whether streaming or not.
    */
    class OPENALPP_API Source : public SourceBase {
    public:
        /**
        * Constructor.
        * Creates the source with the specified position.
        * @param x x coordinate.
        * @param y y coordinate.
        * @param z z coordinate.
        */
        Source(float x = 0.0, float y = 0.0, float z = 0.0);


        /**
        * Constructor.
        * Sets a new (streamed) buffer for the source.
        * The source should _not_ be playing when doing this.
        * @param stream is the new buffer.
        */
        Source( Stream *stream );


        /**
        * Constructor.
        * Creates the source and a buffer with the specified file.
        * @param filename is the name of the file.
        */
        Source(const std::string& filename);

        /**
        * Turn on/off looping.
        * @param loop is true if the source should loop, false otherwise.
        */
        virtual void setLooping(bool loop = true);


        /**
        * Constructor.
        * Creates the source with the specified buffer.
        * @param buffer is the buffer to use.
        */
        Source( Sample *buffer );


        /**
        * Copy constructor.
        */
        Source(const Source &source);


        /**
        * Create a buffer for the source and load a file into it.
        * The source should _not_ be playing when doing this.
        * @param filename is the name of the file.
        */
        void setSound(const std::string& filename);

        /**
        * Sets a new buffer for the source.
        * The source should _not_ be playing when doing this.
        * @param buffer is the new buffer.
        */
        void setSound( Sample *buffer);

        /**
        * Sets a new (streamed) buffer for the source.
        * The source should _not_ be playing when doing this.
        * @param stream is the new buffer.
        */
        void setSound(Stream *stream);

        //void setSound(const Stream *stream);

        /**
        * Gets the buffer associated with the source.
        * @return the buffer.
        */
        const SoundData *getSound() const;

        /**
        * Play a file on the source.
        * This will change the source's buffer.
        * @param filename is the name of the file to play.
        */
        void play(const std::string& filename);

        /**
        * Play a buffer on the source.
        * This will change the source's buffer.
        * @param buffer is the buffer to play.
        */
        void play( Sample *buffer);

        /**
        * Play a stream on the source.
        * This will change the source's buffer.
        * @param stream is the stream to play.
        */
        void play( Stream *stream);

        /**
        * Play this source.
        * This is only here, because the above Play(...) hides SourceBase::Play()
        */
        void play();

        /**
        * Stop this source.
        * This is needed here for streaming sources...
        */
        void stop();

        /**
        * Pause this source.
        * This is needed here for streaming sources...
        */
        void pause();


        /**
        * Seek this source to specified time
        * (streaming sources)
        */
        void seek(float time_s);

        /**
        * Check if the source is streaming.
        * @return true if the source is streaming, false otherwise.
        */
        bool isStreaming();

        /**
        * Assignment operator.
        */
        Source &operator=(const Source &source);

    protected:
        /**
        * Destructor.
        */
        virtual ~Source();

    private:
        /**
        * Pointer to the SoundData (buffer) associated with this source.
        */
        osg::ref_ptr<SoundData> sounddata_;
    };

}

#endif /* OPENALPP_SOURCE_H */
