/**
 * OpenAL++ - an object oriented toolkit for spatial sound
 * Copyright (C) 2002 VRlab, Umeå University
 * bundled with
 * osgAudio - OpenSceneGraph Audio Library
 * (C) Copyright 2009-2011 by Kenneth Mark Bryden
 * (programming by Chris 'Xenon' Hanson, AlphaPixel, LLC xenon at alphapixel.com)
 * based on a fork of:
 * Osg AL - OpenSceneGraph Audio Library
 * Copyright (C) 2004 VRlab, Umeå University
 *
 * OpenAL++ was created using the libraries:
 *                 OpenAL (http://www.openal.org), 
 *              PortAudio (http://www.portaudio.com/), and
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
 * Example where sound is streamed from a microphone.
 */
#include "openalpp/alpp.h"
#include <iostream>

using namespace openalpp;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

int main() {

#ifdef ALPP_USE_PORTAUDIO


  try {
    InputDevice mic;

    Source source(mic);
    source.setGain(2);
	  //source.SetLooping();
    source.play();
    usleep(3000*1000);

    float limits[2] = {0.5,-0.5};
    //float delay=10;
    //float time=0,angle=0;
    
    source.setPosition(limits[0],0.0,0.0);
	  source.play();

    //const int no_laps=5;

    std::cerr << "Moving sound 5 times..." << std::endl;

    // Do a cheat time loop.
#if 0
    while(angle<(M_PI*no_laps)) {
	    usleep(delay*1000); // Wait for delay milliseconds

      time +=delay/1000; // Calculate the time in the loop
      angle=M_PI *time;  // What is the resulting angle
      
      // Calculate a new position
      std::cerr << cos(angle) << ", " << sin(angle) << std::endl;
      source.setPosition(limits[0]*cos(0),0.0,limits[1]*sin(0));
	  }
#else
    std::cerr << "Moved to the left" << std::endl;
    source.setPosition(-0.5,0.0,0);
    usleep(3000*1000);
    std::cerr << "Moved to the right" << std::endl;
    source.setPosition(+0.5,0.0,0);
    usleep(3000*1000);
#endif
  } catch(Error e) {
    std::cerr << e << "\n";
    return -1;
  }
 

  std::cerr << "Press return to continue\n";

  std::cin.get();
#else
    std::cerr << "Use of portaudio is not compiled to the project" << std::endl;
    std::cerr << "Look into the config.h and modify that if you inted to use PortAudio" << std::endl;
#endif

  return 0;

}

