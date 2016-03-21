/* -*-c++-*- */
/**
 * osgAudio - OpenSceneGraph Audio Library
 * (C) Copyright 2009-2011 by Kenneth Mark Bryden
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

#include <openalpp/Stream.h>
#include <openalpp/NetStream.h>

namespace openalpp {

Stream::Stream() throw (NameError) : SoundData() {
  throw InitError("No streaming support compiled in.");
}

Stream::Stream(const Stream &stream) : SoundData((const SoundData &)stream) {
}

Stream &Stream::operator=(const Stream &) {
  return *this;
}

Stream::~Stream() {
}

void Stream::record(ALuint) {
}

void Stream::stop(ALuint) {
}

/*NetStream::NetStream(ost::UDPSocket *socket,ost::TCPStream *controlsocket) 
  : Stream() {
}

NetStream::NetStream(ost::UDPSocket *socket,SampleFormat format,
		     unsigned int frequency,unsigned int packetsize) 
  : Stream() {
}*/


NetStream::NetStream(const NetStream &stream) : Stream((const Stream &)stream){
}

NetStream::~NetStream() {
}

NetStream &NetStream::operator=(const NetStream&) {
  return *this;
}

}
