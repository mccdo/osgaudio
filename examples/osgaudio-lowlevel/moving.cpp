/* -*-c++-*- */
/**
 * osgAudio - OpenSceneGraph Audio Library
 * (C) Copyright 2009-2012 byKenneth Mark Bryden
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
#include <osgAudio/Source.h>
#include <osgAudio/AudioEnvironment.h> // necessary for 3D spatial audio update()

#include <iostream>

// Now make sure M_PI is defined:
#define _USE_MATH_DEFINES 
#include <cmath>
#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795029
#endif

using namespace osgAudio;

// usleep stub
#if defined(WIN32) && !defined (OPENALPP_WINDOWSSTUFF_H)
#include <windows.h>
    inline void usleep( int x ) { Sleep( x /1000 ); };
#endif // defined(WIN32) && !defined (OPENALPP_WINDOWSSTUFF_H)

int main() 
{
    try 
    {
        osgAudio::AudioEnvironment::instance()->init();
        
        osg::ref_ptr<Source> beesound = new Source("bee.wav");
        beesound->setGain(1);
        beesound->setLooping();

        float limits[2] = {5,-15};
        float delay=10;
        float time=0,angle=0;

        beesound->setPosition(limits[0],0.0,0.0);
        beesound->play();

        const int no_laps=5;

        std::cerr << "Moving sound 5 laps..." << std::endl;

        // Do a cheat time loop.
        while(angle<(M_PI*2.0*no_laps)) 
        {
            usleep(delay*1000); // Wait for delay milliseconds

            time +=delay/1000; // Calculate the time in the loop
            angle=M_PI *time;  // What is the resulting angle

            // Calculate a new position
            beesound->setPosition(limits[0]*cos(angle),0.0,limits[1]*sin(angle));
            osgAudio::AudioEnvironment::instance()->update(); // update 3D spatial state
        }
    }
    catch(Error e) 
    {
        std::cerr << e << "\n";
        return -1;
    }
    return 0;
}
  
