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

#include <openalpp/FileStreamUpdater.h>

using namespace openalpp;

const char *getOggVorbisErrorMessage(int e)
{
    switch(e) {
    case (OV_ENOSEEK):
        return "Bitstream is not seekable.";
    case (OV_EINVAL):
        return "Invalid argument value; possibly called with an OggVorbis_File structure that isn't open.";
    case (OV_EREAD):
        return "A read from media returned an error.";
    case (OV_EFAULT):
        return "Internal logic fault; indicates a bug or heap/stack corruption.";
    case (OV_EBADLINK):
        return "Invalid stream section supplied to libvorbisfile, or the requested link is corrupt.";
    default:
        return "Invalid error code specified";
    }
    return "";
}

FileStreamUpdater::FileStreamUpdater(
                                     OggVorbis_File *oggfile,
                                     const ALuint buffer1,ALuint buffer2,
                                     ALenum format,unsigned int frequency,
                                     unsigned int buffersize)
                                     : StreamUpdater(buffer1,buffer2,format,frequency), 
                                     buffersize_(buffersize),
                                     oggfile_(oggfile),
                                     looping_(false),
                                     seekPending_(false),
                                     seekTime_(0)
{

    setCancelModeAsynchronous();

    buffer_ = new ALshort[buffersize_/sizeof(ALshort)];
}

FileStreamUpdater::~FileStreamUpdater() 
{
    // call update to do any remaining buffer deallocation
    StreamUpdater::update(buffer_, 0); 

    if (oggfile_)
    {
        ov_clear(oggfile_);
        delete oggfile_;
    }
    delete buffer_;
    buffer_=0L;
}

void FileStreamUpdater::run() 
{

    bool eofReached = false;

    runmutex_.lock();
    while(!shouldStop())
    {

        unsigned int count=0;
        int stream=0;

        while (count < buffersize_)
        {
            long amt;

            if (seekPending_)
            {
                seekNow(seekTime_);
                seekPending_ = false;
                count = 0;
                continue;

            }

            if (!eofReached)
            {
                amt = ov_read(oggfile_,&((char *)buffer_)[count],
                    buffersize_-count,
                    0,2,1,&stream);
            }
            else
            {
                amt = 0;
            }


            // We must break if:
            // * An error occurred
            // * We hit EOF and the file was not looping 
            // * We hit EOF and the file was looping, but we couldn't loop...
            if (amt > 0)
            {
                eofReached = false;
            }
            else if (amt == 0) 
            {
                //if (!eofReached) fprintf(stdout, "EOF reached\n");
                eofReached = true;
                if (looping_)
                {
                    if(!ov_seekable(oggfile_))
                        break;
                    if(seekNow(0.0)) 
                        eofReached = false;
                    else
                        break;

                }
            }
            else
            {
                std::cerr << "FileStreamUpdater::run() - ov_read error" << std::endl;
                break;
            }


            if (amt > 0)
            {
                count += amt;
            }


        }


        update(buffer_, count);
        //setSleepTime(10*1000);
        //sleep();

        //std::cerr << "FileStreamUpdater" << std::endl;

        runmutex_.unlock();
    }

    runmutex_.unlock();
}


void FileStreamUpdater::seek(float time_s)
{
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(runmutex_);

    seekTime_ = time_s;
    seekPending_ = true;
}

bool FileStreamUpdater::seekNow(float time_s)
{
    if ((oggfile_) && ov_seekable(oggfile_))
    {
        int s = ov_time_seek(oggfile_, time_s);
        if (s) {
            const char *str = getOggVorbisErrorMessage(s);
            std::cerr << "Error seeking oggstream: " << str << std::endl;
            return false;
        }
        return true;
    }
    return false;
}

void FileStreamUpdater::setLooping(bool loop) {
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(runmutex_);
    looping_=loop;
}
