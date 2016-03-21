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

#include <openalpp/DeviceUpdater.h>

using namespace openalpp;


ALenum ALFormat(SampleFormat format) {
    switch(format) {
    case(Mono8):
        return AL_FORMAT_MONO8;
    case(Mono16):
        return AL_FORMAT_MONO16;
    case(Stereo8):
        return AL_FORMAT_STEREO8;
    case(Stereo16):
        return AL_FORMAT_STEREO16;
    }
    return 0;
}

DeviceUpdater::DeviceUpdater(int device, unsigned int frequency,unsigned int bufferSize,
                             SampleFormat format, ALuint buffer1,ALuint buffer2) 
                             :  StreamUpdater(buffer1,buffer2,ALFormat(format),frequency), pCaptureDevice_(0L),
                             totalDataSize_(0){



                                 int nchannels;
                                 int bitsPerSample;
                                 switch(format) {
    case(Mono8):
        format_=AL_FORMAT_MONO8;
        nchannels=1;
        bitsPerSample=8;
        break;
    case(Mono16):
        format_=AL_FORMAT_MONO16;
        bitsPerSample=16;
        nchannels=1;
        break;
    case(Stereo8):
        format_=AL_FORMAT_STEREO8;
        nchannels=2;
        bitsPerSample=16;
        break;
    case(Stereo16):
        format_=AL_FORMAT_STEREO16;
        nchannels=2;
        bitsPerSample=32;
        break;
                                 }
                                 blockAlign_ = nchannels * bitsPerSample / 8;

                                 // Calculate the maximum sleeptime we can have depending on the data generated and
                                 // the buffersize
                                 float max_sleep_time = 8.0*bufferSize/(bitsPerSample * frequency);

                                 // Set the idle time to be slightly less than that to release the CPU
                                 setSleepTime(max_sleep_time * 0.2);

                                 const char *szDefaultCaptureDevice = alcGetString(NULL, ALC_CAPTURE_DEFAULT_DEVICE_SPECIFIER);
                                 std::cerr << "Default capture device:  " << szDefaultCaptureDevice << std::endl;

                                 pCaptureDevice_ = alcCaptureOpenDevice(szDefaultCaptureDevice, frequency, format_, bufferSize);
                                 if (pCaptureDevice_)
                                 {
                                     std::cerr << "Opened '" << alcGetString(pCaptureDevice_, ALC_CAPTURE_DEVICE_SPECIFIER) <<"' Capture Device" << std::endl;
                                 }
                                 else {
                                     throw InitError("DeviceUpdater::init() Unable to initialize capture device");
                                 }


                                 maxtmpbufsize_=bufferSize;
                                 tmpBufSize_ = maxtmpbufsize_;
                                 tmpBuffer_=new char[maxtmpbufsize_];
}

DeviceUpdater::~DeviceUpdater() {
    delete[] tmpBuffer_;
}

void DeviceUpdater::run() {

    int iSamplesAvailable=0;
    int nSamples = tmpBufSize_ / blockAlign_;
    bool done = false;
    do {

        // Start audio capture
        alcCaptureStart(pCaptureDevice_);

        // Find out how many samples have been captured
        alcGetIntegerv(pCaptureDevice_, ALC_CAPTURE_SAMPLES, 1, &iSamplesAvailable);
        if (iSamplesAvailable > nSamples)
        {
            //std::cerr << " got data " << std::endl;
            // Consume Samples
            alcCaptureSamples(pCaptureDevice_, tmpBuffer_, nSamples);

            // Record total amount of data recorded
            enter();

            done=update(tmpBuffer_,tmpBufSize_);
            totalDataSize_ += tmpBufSize_;
            leave();
        }
        YieldCurrentThread();
        OpenThreads::Thread::microSleep(10*1000);
        //std::cerr << " capturing data " << std::endl;

    } while(!shouldStop() && !done);
    alcCaptureStop(pCaptureDevice_);

}

/*void DeviceUpdater::CopyInput(void *tempbuffer,int length) {
if(!tempbuffer)
return;
length*=bytesperframe_;
if(length>maxtmpbufsize_)
tmpbufsize_=maxtmpbufsize_;
else
tmpbufsize_=length;
memcpy(tmpbuffer_,tempbuffer,tmpbufsize_);
}

*/
void DeviceUpdater::stop()
{
    StreamUpdater::stop();
    alcCaptureStop(pCaptureDevice_);
}
