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

#ifndef OPENALPP_STREAMUPDATER_H
#define OPENALPP_STREAMUPDATER_H 1


#include <vector>

#ifdef WIN32
// Ignore the dll interface warning using std::vector members
#pragma warning(disable : 4251)
#endif

//extern "C" {
#include <al.h>
//}

#include <openalpp/Export.h>


#include <openalpp/AudioBase.h>
#include <openalpp/Error.h>
#include <openalpp/windowsstuff.h>
#include <OpenThreads/Thread>
#include <OpenThreads/Mutex>
#include <osg/Referenced>
#include <OpenThreads/ReentrantMutex>
#include <OpenThreads/Block>

namespace openalpp {

    /**
    * Base class for (threaded) updating of stream buffers.
    */
    class OPENALPP_API StreamUpdater : public OpenThreads::Thread, public OpenThreads::ReentrantMutex, public osg::Referenced {

    public:
        /**
        * Constructor.
        * @param buffer1 and...
        * @param buffer2 are the buffers used for double-buffered streaming.
        * @param format is the (OpenAL) format of the sound.
        * @param frequency is the frequency of the sound.
        */
        StreamUpdater(ALuint buffer1,ALuint buffer2,
            ALenum format,unsigned int frequency);


        /**
        * Add a source to the stream.
        * @param sourcename is the OpenAL name of the source.
        */
        void addSource(ALuint sourcename);

        /**
        * Remove a source from the stream.
        * @param sourcename is the OpenAL name of the source.
        */
        void removeSource(ALuint sourcename);

        /**
        * Seeks to specified time
        */
        virtual void seek(float time_s) {};


        /**
        * Tell this StreamUpdater thread to wait until some thread call its release
        */
        void hold() { m_playEvent.reset(); } 

        /**
        * Release this thread.
        */
        void release() { m_playEvent.release(); }

        /**
        * Update the stream.
        * I.e. add new data to play.
        * @param buffer is a pointer to sound data.
        * @param length is the length of the sound data (in bytes).
        * @return done flag. I.e. stoprunning_.
        */
        bool update(void *buffer,unsigned int length); 

        /**
        Tell the thread to stop executing, also release it if it is waiting to play.
        */
        virtual void stop() { stoprunning_ = true; release(); }

        /**
        @return true if the stop method has been called

        */
        bool shouldStop() const { return stoprunning_; }

        /**
        * Inherited from Thread.
        * Is called after run() finishes, and deletes this.
        */
        void cancelCleanup();

        /**
        */
        void setSleepTime(int mseconds) { sleepTime_ = mseconds; }

    protected:

        /**
        * Remove any sources that the user has scheduled for removal.
        */
        void processRemovedSources();

        /**
        * Add any sources that the user has scheduled for adding.
        */
        void processAddedSources();

        /**
        * Destructor.
        */
        virtual ~StreamUpdater();

        /**
        *  Wait for someone to call release. This indicates that we should start playing a stream
        */
        void waitForPlay() { m_playEvent.block(); }

        /**
        * Names of the buffers to update.
        */
        ALuint buffers_[2];

        /**
        * OpenAL format of the sound data.
        */
        ALenum format_;

        /**
        * Frequency of the sound data.
        */
        unsigned int frequency_;

        /**
        * Source to update.
        */

        std::vector<ALuint> sources_,newsources_,removesources_;
        /**
        * Flag for when Run should stop running.
        */ 
        bool stoprunning_;


        /**
        * Mutex for stoprunning_.
        */
        OpenThreads::ReentrantMutex runmutex_;

        /**
        * Time to sleep in run method, i.e. sleeptime to reduce CPU usage
        */
        int sleepTime_;
        void sleep() { OpenThreads::Thread::microSleep(sleepTime_); }


        /** Event signaling that source is playing
        *
        */
        OpenThreads::Block m_playEvent;
    };

}

#endif /* OPENALPP_STREAMUPDATER_H */
