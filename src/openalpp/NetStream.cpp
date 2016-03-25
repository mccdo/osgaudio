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
#ifdef ALPP_USE_NETSTREAM


#include <openalpp/Netupdater.h>
#include <openalpp/Netstream.h>
#include <openalpp/Sample.h>

namespace openalpp {

    NetStream::NetStream(ost::UDPSocket *socket,ost::TCPStream *controlsocket) 
        : Stream() {
            unsigned int frequency=11025,buffersize=2048;
            ALenum format=AL_FORMAT_MONO8;
            if(controlsocket) {
                *controlsocket >> format;
                *controlsocket >> frequency;
                *controlsocket >> buffersize;
            }
            updater_=new NetUpdater(socket,controlsocket,
                buffername_,buffer2_->GetName(),
                format,frequency,buffersize*SampleSize(format));
    }

    NetStream::NetStream(ost::UDPSocket *socket,SampleFormat format,
        unsigned int frequency,unsigned int buffersize) 
        : Stream() {
            ALenum alformat=0;
            switch(format) {
    case(Mono8):
        alformat=AL_FORMAT_MONO8;
        break;
    case(Mono16):
        alformat=AL_FORMAT_MONO16;
        break;
    case(Stereo8):
        alformat=AL_FORMAT_STEREO8;
        break;
    case(Stereo16):
        alformat=AL_FORMAT_STEREO16;
        break;
            }
            updater_=new NetUpdater(socket,NULL,buffername_,buffer2_->GetName(),
                alformat,frequency,buffersize*SampleSize(format));
    }


    NetStream::NetStream(const NetStream &stream) : Stream((const Stream &)stream){
    }

    NetStream::~NetStream() {
    }

    NetStream &NetStream::operator=(const NetStream &stream) {
        if(&stream!=this) {
            Stream::operator=((const Stream &)stream);
        }
        return *this;
    }

}

#endif