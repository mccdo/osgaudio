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

#ifndef OPENALPP_AUDIOBASE_H
#define OPENALPP_AUDIOBASE_H 1

#include <cstdlib>

//extern "C" {
#include <al.h>
#include <AL/alut.h>
#include <alc.h>
//}

#include <osg/Referenced>
#include <osg/ref_ptr>

#include <openalpp/Export.h>
#include <openalpp/Error.h>
#include <openalpp/windowsstuff.h>


/**
* Namespace for OpenAL++.
*/
/// C++ wrapper for OpenAL with additional functionality of handling ogg-vorbis as streams.
namespace openalpp {

    /**
    * Format for sound data. Mono/Stereo, 8 or 16 bits.
    */
    enum SampleFormat {Mono8,Stereo8,Mono16,Stereo16};

    /** 
    * Base class for environment, listener and source classes.
    * Takes care of initialisation/shutdown of anything necessary (e.g. ALut)
    */
    class OPENALPP_API AudioBase : public osg::Referenced {
        /**
        * Counter for #instances for environment, listener and source classes.
        * Used to determine when init and shutdown functions should be called
        */
        static int instances_;

        /**
        * Pointer to device.
        */
        static ALCdevice *device_;

        /**
        * Pointer to context.
        */
#ifndef WIN32
#if OPENAL_VERSION < 2007
        static void *context_;
#else // OPENAL_VERSION < 2007
        static ALCcontext *context_;
#endif // OPENAL_VERSION < 2007
#else
        static struct ALCcontext_struct *context_;
#endif
    protected:
        /**
        * Constructor.
        * @param frequency is the output frequency, in Hz.
        * @param refresh is the refresh rate, in Hz.
        * @param is a flag for synchronous context. Values <0 indicates that the
        * default should be used.
        */
        AudioBase(int frequency=-1,int refresh=-1,int synchronous=-1, bool displayInitMsgs=false )
            throw (InitError);

        /**
        * Destructor.
        */
        virtual  ~AudioBase();

        /**
        * Flag for whether reverb has been initiated.
        * Reverb can be initiated with AudioEnviroment::InitiateReverb()
        */
        static bool reverbinitiated_;

        /**
        * Set reverb scale.
        * This pointer will be set by AudioEnviroment::InitiateReverb()
        * @param sid is the OpenAL name for the source
        * @param param is the reverb scale. Range [0.0,1.0].
        */
        static void (*alReverbScale)(ALuint sid, ALfloat param);

        /**
        * Set reverb delay.
        * This pointer will be set by AudioEnviroment::InitiateReverb()
        * @param sid is the OpenAL name for the source
        * @param param is the reverb delay. Range [0.0,2.0].
        */
        static void (*alReverbDelay)(ALuint sid, ALfloat param);
    };

    const char *alGetErrorString(ALenum error);

#include <iostream>
#define ALCHECKERROR() \
    { \
    ALenum e; \
    if ((e=alGetError()) != AL_NO_ERROR){ \
    const char *msg = openalpp::alGetErrorString(e); \
    std::cerr << "alError: (" << e << ") " << __FILE__ << " at Line: " << __LINE__  << ": " << msg << std::endl; \
    } \
    }

}

#endif /* OPENALPP_AUDIOBASE_H */
