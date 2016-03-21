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

#include <openalpp/Source.h>

#include <iostream>
#include <string>
#include <vector>

using namespace openalpp;

int main() 
{
  osg::ref_ptr<Sample> sample;

  std::cerr << "Loads a sample at the time, plays it for a while, then loads another one etc.." << std::endl;
  try {

    std::vector<std::string> file_vector;
    
    file_vector.push_back("a.wav");
    file_vector.push_back("high-e.wav");
    file_vector.push_back("low-e.wav");

    osg::ref_ptr<Source> source = new Source;
    unsigned int delay = 500;
    for(int i = 0; i < 10; i++) {
      if (sample.valid()) {
        source->stop();
      }
      std::string file = file_vector[i % file_vector.size()];
      std::cerr << "Loading and playing " << file << " for " << delay/1000.0 << " seconds" << std::endl;
      sample = new Sample(file);
      source->setSound(sample.get());     
      source->setGain(1);
      source->setLooping();
      source->play();
      usleep(delay*1000); // Wait for delay milliseconds
    }
  } catch(Error e) {
    std::cerr << e << "\n";
  }

  return 0;
}
  
