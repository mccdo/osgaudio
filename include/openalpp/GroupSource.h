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

#ifndef OPENALPP_GROUPSOURCE_H
#define OPENALPP_GROUPSOURCE_H 1

#include <vector>
#include <math.h>
#include <stdlib.h>

#include <osg/ref_ptr>

#include <openalpp/Export.h>
#include <openalpp/Source.h>
#include <openalpp/AudioConvert.h>

namespace openalpp {

    /**
    * Class for group sources.
    * Used for mixing together several sources _before_ they are played. This can
    * be used to play more sounds with less processing power. Of course the
    * problem is that the sources cannot be moved separately.
    */
    class OPENALPP_API GroupSource : public SourceBase {
        /**
        * Flag for whether the sources have been mixed yet.
        */
        bool mixed_;

        /**
        * Vector containing the sources in the group.
        */
        std::vector<osg::ref_ptr<Source> > sources_;

        /**
        * OpenAL buffer for the group source.
        */
        ALuint buffer_;

        enum Speaker {Left,Right};

        /**
        * Handles distance attenuation and directional sources.
        * @param source is the source to filter.
        * @return the gain.
        */
        ALfloat filterDistance(ALuint source,Speaker speaker);

        /**
        * Reverb filter.
        * @param source is (a pointer to) the source.
        * @param buffer is (a pointer to) the buffer (=memory area).
        * @param size is the size of the buffer.
        * @param frequency is the frequency of the sound.
        * @return new pointer to buffer.
        */
        ALshort *filterReverb(Source *source,ALshort *buffer,
            ALsizei &size,unsigned int frequency);

        /**
        * Apply filters.
        * @param source is (a pointer to) the source.
        * @param buffer is the buffer containing the sound.
        * @param size is the size of the buffer.
        * @param frequency is the frequency of the sound.
        * @return (new) pointer to buffer.
        */
        ALshort *applyFilters(Source *source,ALshort *buffer,ALsizei &size,
            unsigned int frequency);
    public:
        /**
        * Constructor.
        * Creates the group source at the specified position.
        * @param x x coordinate.
        * @param y y coordinate.
        * @param z z coordinate.
        */
        GroupSource(float x = 0.0, float y = 0.0, float z = 0.0) throw (NameError);

        /**
        * Same as SourceBase::Play, except that this mixes the sources in the
        * group if it haven't been done yet.
        */
        void play() throw (InitError,FileError);

        /**
        * Mix all added sources into one. This function will be called by
        * Play(), if sources have been included since the last time MixSamples()
        * was called, so if you want the source to start playing as fast as
        * possible after the Play()-call, MixSources() should be called
        * separately
        * @param frequency is the frequency that will be used when mixing.
        */
        void mixSources(unsigned int frequency=22050) 
            throw (InitError,FileError,FatalError,MemoryError,ValueError);

        /**
        * Includes a source in the group.
        * Returns an identifier that can be used as an argument to ExcludeSource(). 
        * This identifier is also the OpenAL name for the included source.
        * @param source is (a pointer to) the source to include.
        * @return identifier for the source.
        */
        ALuint includeSource(Source *source) throw (ValueError);

        /**
        * Removes a source from the group.
        * This will of course require the remaining sources to be mixed again.
        * @param source is the source to exclude.
        */
        void excludeSource(const Source &source) throw (NameError);

        /**
        * Removes a source from the group.
        * This will of course require the remaining sources to be mixed again.
        * @param source is the identifier of the source to exclude.
        */
        void excludeSource(ALuint source) throw (NameError);

        /**
        * Copy constructor.
        */
        GroupSource(const GroupSource &groupsource);


        /**
        * Assignment operator.
        */
        GroupSource &operator=(const GroupSource &groupsource);

    protected:
        /**
        * Destructor.
        */
        virtual ~GroupSource();

    };

}

#endif /* OPENALPP_GROUPSOURCE_H */
