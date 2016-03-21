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

#ifndef OPENALPP_DEVICEUPDATER_H
#define OPENALPP_DEVICEUPDATER_H 1


#include <openalpp/StreamUpdater.h>
#include <openalpp/Error.h>
#include <openalpp/Export.h>


namespace openalpp {

    /**
    * Updater for sound input devices (like microphones).
    */
    class  DeviceUpdater : public StreamUpdater {

        OpenThreads::Mutex inputmutex_;

        char *tmpBuffer_;
        int tmpBufSize_, maxtmpbufsize_;
        unsigned int totalDataSize_;
        int blockAlign_;
        ALCdevice        *pCaptureDevice_;
        bool done_;

    public:
        /**
        * Constructor.
        * @param device is the number of the device (as PortAudio reports it).
        * TODO: Change this to something non-portaudio dependent.
        * @param frequency is the desired recording frequency.
        * @param buffersize is the desired buffersize.
        * @param format is the format of the sound.
        * @param buffer1 and..
        * @param buffer2 are the (OpenAL) buffers to use.
        */
        OPENALPP_API DeviceUpdater(int device, unsigned int frequency,unsigned int buffersize,
            SampleFormat format,
            ALuint buffer1,ALuint buffer2);

        /**
        * Destructor.
        */
        OPENALPP_API ~DeviceUpdater();

        /**
        * Inherited from Thread.
        * This will be called when the updater is Start():ed..
        */
        OPENALPP_API void run();

        /**
        * Enter critical section.
        */
        OPENALPP_API inline void enter() {inputmutex_.lock();}

        /**
        * Leave critical section.
        */
        OPENALPP_API inline void leave() {inputmutex_.unlock();}

        /**
        * Copy input from callback to temporary buffer.
        * Update will copy the data from the temporary buffer to a sound buffer.
        * @param tempbuffer is a pointer to the buffer
        * @param length is the length - in bytes - of the buffer
        */
        OPENALPP_API void CopyInput(void *tempbuffer,int length);

        /**
        * Terminates the capturing thread. 
        */
        virtual void stop();

    };

}

#endif /* OPENALPP_DEVICEUPDATER_H */
