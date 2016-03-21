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

#ifndef OPENALPP_NETUPDATER_H
#define OPENALPP_NETUPDATER_H 1

#if ALPP_USE_NETSTREAM 

#include <cc++/socket.h>
#include "stdlib.h"
#include <openalpp/Export.h>
#include <openalpp/StreamUpdater.h>

namespace openalpp {

    /**
    * Updater for streams through sockets.
    */
    class OPENALPP_API NetUpdater : public StreamUpdater {
        /**
        * Socket to receive sound data from.
        */
        ost::UDPSocket *socket_;

        /**
        * Stream to receive control messages from.
        */
        ost::TCPStream *controlsocket_;

        /**
        * Size of sound buffer (in bytes).
        */
        unsigned int buffersize_;
    public:
        /**
        * Constructor.
        * @param socket is the socket to receive sound data from.
        * @param controlsocket is the stream to receive control messages from. Set
        * to NULL if control messages are not used.
        * @param buffer1 and...
        * @param buffer2 are the sound buffers to use.
        * @param format is the (OpenAL) format of the sound data.
        * @param frequency is the frequency of the sound data.
        * @param buffersize is how much data to buffer before adding to the stream.
        */
        NetUpdater(ost::UDPSocket *socket,ost::TCPStream *controlsocket,
            const ALuint buffer1,ALuint buffer2,
            ALenum format,unsigned int frequency,
            unsigned int buffersize);

        /**
        * Inherited from Thread.
        * This will be called when the updater is Start():ed..
        */
        void run();
    };

}

#endif /* OPENALPP_NETUPDATER_H */
#endif