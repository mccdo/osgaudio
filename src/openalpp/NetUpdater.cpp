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

namespace openalpp {

    NetUpdater::NetUpdater(ost::UDPSocket *socket,ost::TCPStream *controlsocket,
        const ALuint buffer1,ALuint buffer2,
        ALenum format,unsigned int frequency,
        unsigned int buffersize) 
        : StreamUpdater(buffer1,buffer2,format,frequency), socket_(socket)
        , controlsocket_(controlsocket), buffersize_(buffersize) {
    }

    void NetUpdater::run() {
        char *buffer=new char[buffersize_];
        unsigned int len=0,received=0;

        runmutex_.enterMutex();
        while(!stoprunning_) {
            runmutex_.leaveMutex();
            // TODO: Make the timeout dependent on how long it would take to play
            // one buffer. If it takes too long, just fill up with zeros and Update.
            if(socket_->isPending(ost::Socket::pendingInput,100)) {
                len=socket_->receive(&(buffer[received]),buffersize_-received);
                received+=len;
                if(received>=buffersize_) {
                    Update(buffer,received);
                    received=0;
                }
            } else {
                // Timeout; fill the buffer with zeros (silence) and Update
                memset(&(buffer[received]),0,buffersize_-received);
                Update(buffer,buffersize_);
                received=0;

                // Take care of messages on the control socket (if it exists)
                if(controlsocket_ && 
                    controlsocket_->isPending(ost::Socket::pendingInput,100)) {
                        char instr[100];
                        *controlsocket_ >> instr;
                        runmutex_.enterMutex();
                        break;
                }
            }
            runmutex_.enterMutex();
        }
        runmutex_.leaveMutex();

        delete []buffer;
    }

}
#endif