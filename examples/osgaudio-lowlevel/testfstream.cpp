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
#include <osgAudio/FileStream.h>
#include <osgAudio/AudioEnvironment.h>

#include <iostream>
#include <vector>
#include <cstdio>

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

void WaitForPlaying(Source* source)
{
    while (source->getState() != osgAudio::Playing)
    {
        usleep(1000);
        int checkCount = 0;
        if (checkCount++ > 1000)
        {
            throw osgAudio::Error("Timed out while waiting for stream to play");
        }
    }
}

void WaitForStopped(Source* source)
{
    while (source->getState() != osgAudio::Stopped)
    {
        usleep(1000);
        int checkCount = 0;
        if (checkCount++ > 1000)
        {
            throw osgAudio::Error("Timed out while waiting for stream to stop");
        }
    }
}

void TestSeek()
{
    std::string numbers("one_ten_b.ogg");
    

    std::vector<float> seekPoints;
    seekPoints.push_back(7.0);
    seekPoints.push_back(5.0);
    seekPoints.push_back(2.0);
    seekPoints.push_back(3.0);

    try 
    {
        osg::ref_ptr<Source> source = new Source;

        source->setSound(new FileStream(numbers));
        source->setAmbient();

        for (size_t k=0; k<seekPoints.size(); ++k)
        {
            fprintf(stdout, "%.0f ", seekPoints[k]);
            source->seek(seekPoints[k]);
            if (k==0)
            {
                source->play();
            }
            usleep(500*1000);
        }
        fprintf(stdout, "\n");

        source->stop();

        WaitForStopped(source.get());

    }
    catch(osgAudio::Error e) 
    {
        std::cerr << e << "\n";
    } 
    catch(...) 
    {
        std::cerr << "Unknown error!\n";
    }
    
}



int main(int argc,char **argv) {
#ifdef WIN32
    // MSVC memory leak detection
    _CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF ); // {,,msvcr71d.dll}_crtBreakAlloc
#endif
    std::string file1("test1.ogg");
    std::string file2("test2.ogg");
    std::string file3("test3.ogg");
    
    TestSeek();
    usleep(500000);
    try 
    {
        osgAudio::AudioEnvironment::instance()->init();

        osg::ref_ptr<Source> source = new Source;

        source->setSound(new FileStream(file1));
        source->setAmbient();
        source->play();

        // Test transition to audio file when first finishes
        std::cout << "Waiting for first audio file to stop" << std::endl;

        WaitForStopped(source.get());
        
        std::cout << "Playing second audio file" << std::endl;

        // For now, the source has to be re-created for stable behavior
        source = new Source; 

        source->setSound(new FileStream(file3));
        source->setAmbient();
        source->play();

        // Test pause and resume
        std::cout << "Press return to pause audio file" << std::endl;
        std::cin.get();

        source->pause();

        std::cout << "Press return to resume audio file" << std::endl;
        std::cin.get();

        source->play();

        // Test switching between files mid-stream
        std::cout << "Press return to switch to next audio file" << std::endl;
        std::cin.get();


        for (int nSwitch = 0; nSwitch < 2; nSwitch++)
        {
            source = new Source;

            source->setSound(new FileStream(file2));
            source->setAmbient();
            source->play();

            std::cout << "Press return to switch to next audio file" << std::endl;
            std::cin.get();

            source->stop();
            WaitForStopped(source.get());

            source = new Source;

            source->setSound(new FileStream(file1));
            source->setAmbient();
            source->play();

            std::cout << "Press return to switch to next audio file" << std::endl;
            std::cin.get();

            source->stop();
            WaitForStopped(source.get());

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
