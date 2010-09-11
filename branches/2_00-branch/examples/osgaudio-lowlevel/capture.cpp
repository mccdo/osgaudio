/* -*-c++-*- */
/**
 * osgAudio - OpenSceneGraph Audio Library
 * Copyright (C) 2010 AlphaPixel, LLC
 * based on a fork of:
 * Osg AL - OpenSceneGraph Audio Library
 * Copyright (C) 2004 VRlab, Ume� University
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
 */
#define _USE_MATH_DEFINES 
#include <cmath>

#include <iostream>
#include <osgAudio/Source.h>
#ifdef COMMENTED_OUT // <<<>>> Capture not currently supported, unknown if it will be anytime soon
#include <osgAudio/Capture.h>
#endif // COMMENTED_OUT

#define CRTDBG_MAP_ALLOC
#include <stdlib.h>
#ifdef WIN32
#include <crtdbg.h>
#endif



using namespace osgAudio;

// usleep stub
#if defined(WIN32) && !defined (OPENALPP_WINDOWSSTUFF_H)
#include <windows.h>
	inline void usleep( int x ) { Sleep( x /1000 ); };
#endif // defined(WIN32) && !defined (OPENALPP_WINDOWSSTUFF_H)


int main(int argc,char **argv) {

  std::cerr << "\n Demonstrates how to record a stream and feed that into a Source\n\n" << std::endl;

#ifdef COMMENTED_OUT // <<<>>> Capture not currently supported, unknown if it will be anytime soon
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
    source = 0;
  } 
  catch(osgAudio::Error e) 
  {
    std::cerr << e << "\n";
  } 
  catch(...) 
  {
    std::cerr << "Unknown error!\n";
  }
#endif // COMMENTED_OUT

  return 0;
}
