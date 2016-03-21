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

#include "stdio.h"
#include <sstream>

#include <openalpp/AudioBase.h>

using namespace openalpp;

AudioBase::AudioBase(int frequency,int refresh,int synchronous, bool displayInitMsgs )
throw (InitError)
{
    if( displayInitMsgs )
        std::cout << "Initializing OpenAL." << std::endl;

    if(!instances_) {
#if OPENAL_VERSION >= 2007
        ALboolean success = alutInitWithoutContext (0L, 0L);
        if (success != AL_TRUE) {
            ALenum error = alutGetError ();
            const char *err_str = alutGetErrorString(error);
            std::ostringstream str;
            str << "Couldn't open device: " << std::string(err_str);
            throw InitError(str.str().c_str());
        }
#endif
        // Open a write (output) device. This should (in theory) make it possible
        // to open a read (input) device later.. 
#ifdef _WIN32
        //char *initString = 0L;//"DirectSound", "DirectSound3D", ;
        char *initStringList = (char*)alcGetString(NULL, ALC_DEVICE_SPECIFIER);
        if (initStringList)
        {
            //std::cout << "sound device " << initStringList << std::endl << "found" << std::endl;
            std::string initStringList2(initStringList);
            std::string deviceAsked = "Generic Hardware";
            if (initStringList2.find(deviceAsked) == std::string::npos)
            {
                if( displayInitMsgs )
                    std::cerr << "No Generic Hardware in: " << initStringList2 << std::endl;
                device_ =alcOpenDevice(initStringList);
            } else {
                device_ =alcOpenDevice((const ALCchar *)deviceAsked.c_str());
                //std::cout << "device_ =alcOpenDevice((const ALCchar *)initStringList);" << std::endl;
            }
        }
        else
        {
            if( displayInitMsgs )
                std::cout << "alcGetString returned NULL" << std::endl;
            device_ = 0;
        }

#else
#if OPENAL_VERSION < 2005
        device_=alcOpenDevice((/*const */ALubyte *)"'((direction \"write\")) '((devices '(alsa sdl native null)))");
#else // OPENAL_VERSION < 2005
        device_=alcOpenDevice("'((direction \"write\")) '((devices '(alsa sdl native null)))");
#endif // OPENAL_VERSION < 2005
        // Second try if previous call failed
        if(!device_)
            device_=alcOpenDevice(NULL);
#endif
        if(!device_)
            throw InitError("OpenAL++: Couldn't open sound device.");
        int attributes[7],i=0;
        attributes[0]=0;
        if(frequency>0) {
            attributes[i++]=ALC_FREQUENCY;
            attributes[i++]=frequency;
            attributes[i]=0;
        }
        if(refresh>0) {
            attributes[i++]=ALC_REFRESH;
            attributes[i++]=refresh;
            attributes[i]=0;
        }
        if(synchronous>0) {
            attributes[i++]=ALC_SYNC;
            attributes[i++]=synchronous;
            attributes[i]=0;
        }
        context_=alcCreateContext(device_,attributes);
        if(!context_ || alcGetError(device_)!=ALC_NO_ERROR) {
            if(context_)
                alcDestroyContext(context_);
            alcCloseDevice(device_);
            throw InitError("Couldn't create context");
        } 
        alcMakeContextCurrent(context_);
        reverbinitiated_=false;

        // Check for EAX 2.0 support
#if OPENAL_VERSION < 2005
        unsigned char szFnName[256];
        ALboolean g_bEAX = alIsExtensionPresent((ALubyte*)"EAX2.0");
#else // OPENAL_VERSION < 2005
        char szFnName[256];
        ALboolean g_bEAX = alIsExtensionPresent("EAX2.0");
#endif // OPENAL_VERSION < 2005
        if (g_bEAX == AL_TRUE)
        {
            sprintf((char*)szFnName, "EAXSet");
            ALvoid *eaxSet = alGetProcAddress(szFnName);
            if (eaxSet == NULL) g_bEAX = AL_FALSE;
        }
        if (g_bEAX == AL_TRUE)
        {
            sprintf((char*)szFnName,"EAXGet");
            ALvoid *eaxGet = alGetProcAddress(szFnName);
            if (eaxGet == NULL) g_bEAX = AL_FALSE;
        }
        if( displayInitMsgs )
        {
            if (g_bEAX == AL_TRUE)
                std::cerr << "Using OpenAL EAX2.0 extension" << std::endl;
            else
                std::cerr << "No OpenAL EAX2.0 extensions available" << std::endl;
        }
    }

    instances_++;
}

AudioBase::~AudioBase() {
    instances_--;
    if(!instances_) {
        //    std::cerr << "All instances removed, closing OpenAL context&device" << std::endl;
        alutExit ();
        alcMakeContextCurrent(NULL);
        alcDestroyContext(context_);
        alcCloseDevice(device_);
    }
}


const char *openalpp::alGetErrorString(ALenum error)
{
    switch (error) {
    case (AL_INVALID_NAME):
        return "Invalid Name parameter passed to AL call.";
    case (AL_INVALID_ENUM):
        return "Invalid parameter passed to AL call.";
    case (AL_INVALID_VALUE):
        return "Invalid enum parameter value.";
    case (AL_INVALID_OPERATION):
        return "Illegal call.";
    case (AL_OUT_OF_MEMORY):
        return "Out of memory.";
    default:
        return "No message associated with error code.";

    }
    return "";
}


// Static members
int AudioBase::instances_=0;
ALCdevice *AudioBase::device_=NULL;
#ifndef WIN32
#if OPENAL_VERSION < 2007
void *AudioBase::context_=NULL;
#else // OPENAL_VERSION < 2007
ALCcontext *AudioBase::context_=NULL;
#endif // OPENAL_VERSION < 2007
#else
struct ALCcontext_struct *AudioBase::context_=NULL;
#endif

