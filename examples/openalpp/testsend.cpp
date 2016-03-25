/**
 * osgAudio - OpenSceneGraph Audio Library
 * (C) Copyright 2009-2011 by Kenneth Mark Bryden
 * (programming by Chris 'Xenon' Hanson, AlphaPixel, LLC xenon at alphapixel.com)
 * based on a fork of:
 * Osg AL - OpenSceneGraph Audio Library
 * Copyright (C) 2004 VRlab, Umeå University
 * OpenAL++ - an object oriented toolkit for spatial sound
 * Copyright (C) 2002 VRlab, Umeå University
 *
 * OpenAL++ was created using the libraries:
 *                 OpenAL (http://www.openal.org), 
 *              PortAudio (http://www.portaudio.com/), and
 *              CommonC++ (http://cplusplus.sourceforge.net/)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Example where a soundfile is loaded using alutLoadWav and streamed over a
 * UDP socket to a receiver that will read the datastream and play it.
 *
 * Note about selecting rate, format and sending rate:
 * Rate is given in samples per second, and format gives the size of one
 * sample. This means that, for example:
 *  If the format is Mono16, we have one channel with 16 bit per channel, i.e.
 *   16 bit or 2 bytes per sample.
 *  Say we have a frequency of 44100 samples per second. 2 byte samples will
 *   mean that we need to stream 88200 bytes per second. 
 *  If we can send (and receive) ten packets per second, each packet should be
 *   at least 8820 bytes.
 *  If we instead had a frequency of 11025, then it would be enough with 2205
 *   bytes per packet.
 * Of course it's a good idea to add a few hundred bytes to those numbers ;)
 */

#include <cc++/thread.h>
#include <cc++/socket.h>
#include <iostream>
#include <stdlib.h>
#include <AL/al.h>
#include <AL/alut.h>
#include "openalpp/windowsstuff.h"

using namespace ost;

int main(int argc,char **argv) {
  ALvoid *data;
  ALsizei bits,freq,size;
  ALenum format;
  ALboolean success;
  // Default packet size (in bytes)
  // Note that this doesn't necessarily have to have anything to do with the
  // buffer size of the receiver. Just set it to something appropriate
  // (depending on the connection), and the receiver will take care of the
  // playing the sound correctly...
  unsigned int packetsize=10000;
  const char defaultfile[]="bee.wav";
  const char *filename;

  if(argc>1)
    filename=argv[1];
  else
    filename=defaultfile;
  if(argc>2)
    packetsize=atoi(argv[2]);

  try {
    success=alutLoadWAV(filename,&data,&format,&size,&bits,&freq);
    std::cerr << "Bits:  " << bits << " Freq: " << freq << std::endl;

    if(success==AL_FALSE) {
      std::cerr << "Error loading " << filename << "\n";
      exit(1);
    }

    UDPSocket socket;
    socket.setPeer(InetHostAddress("127.0.0.1"),33333);

    int totalsent=0;
    while(totalsent<size) {
      // Send data in packets with a 60 ms delay between packets
      if((totalsent+packetsize)>size)
	      packetsize=size-totalsent;
      totalsent+=socket.send((char *)data+totalsent,packetsize);
      ost::Thread::sleep(60);
    }

    free(data);
  } catch(...) {
    std::cerr << "Error caught!\n";
  }

  return 0;
}
