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
#include <openalpp/FileStream.h>

#define CRTDBG_MAP_ALLOC
#include <stdlib.h>
#ifdef WIN32
#include <crtdbg.h>
#endif



using namespace openalpp;


int main(int argc,char **argv) {

  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <ogg-file>" << std::endl;
    return 0;
  }

  try 
    {

      osg::ref_ptr<FileStream> fstream = new FileStream(argv[1]);
      osg::ref_ptr<Source> source = new Source(fstream.get());

      source->setLooping(true);
     //   source->play();

      while(1) {
          std::cerr << "1. play" << std::endl;
          std::cerr << "2. stop" << std::endl;
          std::cerr << "3. rewind" << std::endl;
          std::cerr << "4. pause" << std::endl;

          std::cout << "Press q+return to exit> " << std::endl;


          std::string line;
          std::getline(std::cin, line, '\n');
          

          if (line == "1") {
            source->play();
          }
          else if (line == "2") 
            source->stop();
          else if (line == "3") 
            source->seek(0);
          else if (line=="4")
            source->pause();
          else if (line=="q")
            break;
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
