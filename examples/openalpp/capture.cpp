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
#define _USE_MATH_DEFINES 
#include <cmath>

#include <iostream>
#include <openalpp/Source.h>
#include <openalpp/Capture.h>

#define CRTDBG_MAP_ALLOC
#include <stdlib.h>
#ifdef WIN32
#include <crtdbg.h>
#endif



using namespace openalpp;


int main(int argc,char **argv) {

  std::cerr << "\n Demonstrates how to record a stream and feed that into a Source\n\n" << std::endl;
  try 
  {

    osg::ref_ptr<Capture> input = new Capture(0,44100*0.5,2048*4,Mono16);
    osg::ref_ptr<Source> source = new Source(input.get());

    source->setAmbient();

    float limits[2] = {5,-15};
    float delay=10;
    float time=0,angle=0;
    float pitch = 1;
    while(1) {
      std::cerr << "1. Start streaming from input device" << std::endl;
      std::cerr << "2. Stop streaming" << std::endl;
      std::cerr << "3. Move sound left/right for 3 seconds" << std::endl;
      std::cerr << "+. increase pitch" << std::endl;
      std::cerr << "-. decrease pitch" << std::endl;

      std::cout << "Press q+return to exit> " << std::endl;


      std::string line;
      std::getline(std::cin, line, '\n');


      if (line == "1") {
        source->play();
      }
      else if (line == "2") 
        source->stop();
      else if (line == "+") {
        pitch *= 1.1f;
        std::cerr << "Setting pitch to: " << pitch << std::endl;
        source->setPitch(pitch);

      }
      else if (line=="-"){
        pitch *= 0.9f;
        std::cerr << "Setting pitch to: " << pitch << std::endl;
        source->setPitch(pitch);

      }
      else if (line=="q")
        break;
      else if (line == "3") {
        source->setPosition(limits[0],0.0,0.0);
        std::cerr << "Moving sound 3 laps..." << std::endl;
        const int no_laps=3;

        // Do a cheat time loop.
        while(angle<(M_PI*2.0*no_laps)) {
          usleep(delay*1000); // Wait for delay milliseconds

          time +=delay/1000; // Calculate the time in the loop
          angle=M_PI *time;  // What is the resulting angle

          // Calculate a new position
          source->setPosition(limits[0]*cos(angle),0.0,limits[1]*sin(angle));
        }
        
      }
      else
        std::cerr << "'" << line << "' is an invalid choice" << std::endl;
    }
    source = 0L;
  } 
  catch(openalpp::Error e) 
  {
    std::cerr << e << "\n";
  } 
  catch(...) 
  {
    std::cerr << "Unknown error!\n";
  }


  return 0;
}
