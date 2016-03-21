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
#include <openalpp/AudioEnvironment.h>
#include <openalpp/windowsstuff.h>

using namespace openalpp;

AudioEnvironment::AudioEnvironment( bool displayInitMsgs ) throw (InitError)
  : AudioBase( -1, -1, -1, displayInitMsgs )
{
}

AudioEnvironment::AudioEnvironment(int frequency,int refresh,bool synchronous, bool displayInitMsgs)
throw (InitError)
  : AudioBase(frequency,refresh,synchronous, displayInitMsgs ) {
}

AudioEnvironment::AudioEnvironment(int frequency,int refresh) throw (InitError)
: AudioBase(frequency,refresh) {
}

void AudioEnvironment::setSoundVelocity(float speed) throw(ValueError,FatalError){
    alDopplerVelocity(speed);
    ALenum error;
    if((error=alGetError())!=AL_FALSE)
        switch(error) {
      case(AL_INVALID_VALUE):
          throw ValueError((const char *)alGetString(error));
          break;
      default:
          throw FatalError("Unknown error in AudioEnvironment::SetSoundSpeed()");
          break;
    }
}

float AudioEnvironment::getSoundVelocity() throw (FatalError) {
    ALfloat speed;
    alGetFloatv(AL_DOPPLER_VELOCITY,&speed);
    if(alGetError()!=AL_FALSE)  // This isn't strictly necessary...
        throw FatalError("Unknown error in AudioEnviroment::GetSoundVelocity()");
    return speed;
}

void AudioEnvironment::setDopplerFactor(float factor) 
throw (ValueError,FatalError) {
    alDopplerFactor(factor);
    ALenum error;
    if((error=alGetError())!=AL_FALSE)
        switch(error) {
      case(AL_INVALID_VALUE):
          throw ValueError((const char *)alGetString(error));
          break;
      default:
          throw FatalError("Unknown error in AudioEnvironment::SetDopplerFactor()");
          break;
    }
}

float AudioEnvironment::getDopplerFactor() throw (FatalError) {
    ALfloat(factor);
    alGetFloatv(AL_DOPPLER_FACTOR,&factor);
    if(alGetError()!=AL_FALSE)  // This isn't strictly necessary...
        throw FatalError("Unknown error in AudioEnvironment::GetDopplerFactor()");
    return factor;
}

void AudioEnvironment::setGain(float gain) {
    alListenerf(AL_GAIN,gain);
    ALenum error=alGetError();
    if(error!=AL_FALSE)
        switch(error) {
      case(AL_INVALID_VALUE):
          throw ValueError("Invalid value for gain");
          break;
      default:
          throw FatalError("Error trying to set gain!");
    }
}

float AudioEnvironment::getGain() throw (FatalError) {  
    ALfloat gain;
    alGetListenerf(AL_GAIN,&gain);
    if(alGetError()!=AL_FALSE)          // This isn't strictly necessary...
        throw FatalError("Unknown error in AudioEnvironment::GetGain()");
    return gain;
}

void AudioEnvironment::setDistanceModel(DistanceModel model)
throw (FatalError){
    switch(model) {
    case(None):
        alDistanceModel(AL_NONE);
        break;
    case(InverseDistance):
        alDistanceModel(AL_INVERSE_DISTANCE);
        break;
    case(InverseDistanceClamped):
        alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);
        break;
    case(Linear):
        alDistanceModel(AL_LINEAR_DISTANCE);
        break;
    default:
        throw FatalError("Unknown model in AudioEnvironment::SetDistanceModel()");
    }
    if(alGetError()!=AL_FALSE)
        throw FatalError("alDistanceModel failed in AudioEnvironment::SetDistanceModel()");
}

DistanceModel AudioEnvironment::getDistanceModel() throw (FatalError) {
    int almodel;
    alGetIntegerv(AL_DISTANCE_MODEL,&almodel);
    switch(almodel) {
    case(AL_NONE):
        return None;
    case(AL_INVERSE_DISTANCE):
        return InverseDistance;
    case(AL_INVERSE_DISTANCE_CLAMPED):
        return InverseDistanceClamped;
    case(AL_LINEAR_DISTANCE):
        return Linear;
    default:
        throw FatalError("Unknown distance model in AudioEnvironment::GetDistanceModel");
    }
}

// Static fields in AudioBase
bool AudioBase::reverbinitiated_;
void (*AudioBase::alReverbScale)(ALuint sid, ALfloat param);
void (*AudioBase::alReverbDelay)(ALuint sid, ALfloat param);

void AudioEnvironment::initiateReverb() throw (InitError) {
    if (reverbinitiated_)
        return;

    alReverbScale=(void (*)(ALuint sid, ALfloat param))
#if OPENAL_VERSION < 2005
        alGetProcAddress((ALubyte *)"alReverbScale_LOKI");
#else // OPENAL_VERSION < 2005
        alGetProcAddress("alReverbScale_LOKI");
#endif // OPENAL_VERSION < 2005
    alReverbDelay=(void (*)(ALuint sid, ALfloat param))
#if OPENAL_VERSION < 2005
        alGetProcAddress((ALubyte *)"alReverbDelay_LOKI");
#else // OPENAL_VERSION < 2005
        alGetProcAddress("alReverbDelay_LOKI");
#endif // OPENAL_VERSION < 2005
    if(!(alReverbScale && alReverbDelay)) {
        throw InitError("Couldn't initiate reverb");
    } else
        reverbinitiated_=true;
}

