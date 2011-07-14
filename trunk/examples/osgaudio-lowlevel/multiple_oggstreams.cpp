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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
 */
#define _USE_MATH_DEFINES 
#include <cmath>

#include <iostream>
#include <osgAudio/Source.h>
#include <osgAudio/FileStream.h>
#include <osgAudio/AudioEnvironment.h>

#include <vector>

using namespace osgAudio;


int main(int argc,char **argv) {

  try 
  {
      osgAudio::AudioEnvironment::instance()->init();
    typedef std::vector<osg::ref_ptr<Source> > SourceVector;
    SourceVector sourceVector;
    for (int i=0; i < 8; i++)
    {
      osg::ref_ptr<FileStream> fstream;
      int n=0;
      if (i%2) {
        fstream = new FileStream("right.ogg");
        n=20;
      }
      else {
        fstream = new FileStream("left.ogg");
      }
      osg::ref_ptr<Source> source = new Source(fstream.get());

      source->setLooping(true);
      source->setPosition(-10+n+i*1,0,0);
      sourceVector.push_back(source);
    }

    SourceVector::iterator it;
    for (it=sourceVector.begin(); it != sourceVector.end(); it++) {
      (*it)->play();
    }

    std::cout << "Press return to exit" << std::endl;
    std::cin.get();
  } 
  catch(osgAudio::Error e) 
  {
    std::cerr << e << "\n";
  } 
  catch(...) 
  {
    std::cerr << "Unknown error!\n";
  }


  return 0;
}
