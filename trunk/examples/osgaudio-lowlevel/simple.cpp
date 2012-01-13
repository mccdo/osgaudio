/* -*-c++-*- */
/**
 * osgAudio - OpenSceneGraph Audio Library
 * (C) Copyright 2009-2012 byKenneth Mark Bryden
 * (programming by Chris 'Xenon' Hanson, AlphaPixel, LLC xenon at alphapixel.com)
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

#include <iostream>
#include <osgAudio/Source.h>
#include <osgAudio/Sample.h>
#include <osgAudio/AudioEnvironment.h>

using namespace osgAudio;


int main(int argc,char **argv) {

  std::string file;
  file = "bee.wav";

  if (argc > 1)
    file = argv[1];

  std::cout << "Playing " << file << "\n";

  try {
      osgAudio::AudioEnvironment::instance()->init();
    osg::ref_ptr<Source> bgsound = new Source; //(file);   // Create source and load sound
    bgsound->stop();
    osg::ref_ptr<Sample> sample = new Sample(file);
    bgsound->setSound(sample.get());
    bgsound->setGain(0.6f);      // Lower gain (volume) 
    bgsound->setLooping();
    bgsound->setAmbient();      // Make sound ambient (i.e. not attenuated)
    bgsound->play();          // Start playing
    
    
    std::cerr << "Press return to stop: " << std::endl;

    std::string str;
    std::getline(std::cin, str);
    bgsound->stop();
    std::cerr << "Done " << std::endl;

  } catch(Error e) {
    std::cerr << e << "\n";
    return -1;
  }
  return 0;
}
