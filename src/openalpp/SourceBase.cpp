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

#include <sstream>

#include <openalpp/SourceBase.h>

/**
* TODO: Should error checking be optional?
* (a static bool throwflag in AudioBase)?
*/

using namespace openalpp;

void SourceBase::init() throw (MemoryError,NameError) {
    alGenSources(1,&sourcename_);
    if(alGetError()!=AL_FALSE)
        throw NameError("Couldn't generate source name");

    nlinkedsources_=1;
    alloclinkedsources_=2;
    linkedsources_ = new ALuint[alloclinkedsources_];

    if(!linkedsources_)
        throw MemoryError("new failed");
    linkedsources_[0]=sourcename_;
    reverbscale_=0.25;
    reverbdelay_=0.0;
    streaming_=false;
}

SourceBase::SourceBase() throw (MemoryError,NameError) : PositionedObject() {
    init();
}

SourceBase::SourceBase(float x,float y,float z)
throw (MemoryError,NameError) : PositionedObject() {

    init();
    setPosition(x,y,z);
}

SourceBase::~SourceBase() {
    stop();

    alDeleteSources(1,&sourcename_);
    delete [] linkedsources_;

    ALenum status = alGetError();
    if (status != AL_FALSE)
    {

        std::cerr << "~SourceBase() - Warning - Error deleting sources\n";
        std::cerr << alGetErrorString(status) << std::endl;
    }
}

SourceBase::SourceBase(const SourceBase &sourcebase)
: PositionedObject(sourcebase) {

    if(this==&sourcebase)
        return;

    *this = sourcebase;
}

SourceBase &SourceBase::operator=(const SourceBase &sourcebase) {
    if(this==&sourcebase)
        return *this;

    float a,b,c;
    sourcebase.getPosition(a,b,c);
    setPosition(a,b,c);
    sourcebase.getVelocity(a,b,c);
    setVelocity(a,b,c);
    setPitch(sourcebase.getPitch());
    sourcebase.getDirection(a,b,c);
    setDirection(a,b,c);
    sourcebase.getSoundCone(a,b,c);
    setSoundCone(a,b,c);
    sourcebase.getMinMaxGain(a,b);
    setMinMaxGain(a,b);
    setReferenceDistance(sourcebase.getReferenceDistance());
    setRolloffFactor(sourcebase.getRolloffFactor());
    setMaxDistance(sourcebase.getMaxDistance());
    setLooping(sourcebase.isLooping());
    setGain(sourcebase.getGain());

    return *this;
}

void SourceBase::play() {
    alSourcePlayv(nlinkedsources_,linkedsources_);
    int status = alGetError();
    if (status != AL_NO_ERROR)
    {
        std::cerr << "SourceBase::play() - Warning - Error playing sources\n";
        std::cerr << alGetErrorString(status) << std::endl;
    }
}

void SourceBase::pause() 
{
    if (getState() != Playing) return; // only pause when playing

    alSourcePausev(nlinkedsources_,linkedsources_);

    int status = alGetError();
    if (status != AL_NO_ERROR)
    {
        std::cerr << "SourceBase::pause() - Warning - Error pausing sources\n";
        std::cerr << alGetErrorString(status) << std::endl;
    }
}

void SourceBase::stop() {

    SourceState state = getState();

    if (state == Stopped) return; // already stopped

    alSourceStopv(nlinkedsources_,linkedsources_);

    int status = alGetError();
    if (status != AL_FALSE)
    {
        std::cerr << "SourceBase::stop() - Warning - Error stopping sources\n";
        std::cerr << alGetErrorString(status) << std::endl;
    }
}

void SourceBase::rewind() {
    alSourceRewindv(nlinkedsources_,linkedsources_);
}

SourceState SourceBase::getState() const {
    ALint state;
    alGetSourceiv(sourcename_,AL_SOURCE_STATE,&state);

    if (ALenum errorCode = alGetError() != AL_FALSE)
    {
        /** error occurs here when switching files
        fprintf(stderr, "SourceBase::getState() - Warning - Error getting state (%d)\n",
        errorCode);
        */
    }


    switch(state) {
    case(AL_INITIAL):
        return Initial;
    case(AL_PLAYING):
        return Playing;
    case(AL_PAUSED):
        return Paused;
    case(AL_STOPPED):
        return Stopped;
    default:
        throw FatalError("AL_SOURCE_STATE is unknown");
    }
}

void SourceBase::setLooping(bool loop) {
    if(loop && !streaming_)
        alSourcei(sourcename_,AL_LOOPING,AL_TRUE);
    else
        alSourcei(sourcename_,AL_LOOPING,AL_FALSE);
}

bool SourceBase::isLooping() const {
    ALint looping;
    alGetSourceiv(sourcename_,AL_LOOPING,&looping);
    return (looping==AL_TRUE);
}

bool SourceBase::isPaused() const
{
    return (getState() == Paused);
}

void SourceBase::setDirection(float directionx, float directiony, float directionz) {
    alSource3f(sourcename_,AL_DIRECTION,directionx,directiony,directionz);
}

void SourceBase::getDirection(float &directionx, float &directiony, float &directionz) const {
    ALfloat direction[3];
    alGetSourcefv(sourcename_,AL_DIRECTION,direction);
    directionx=direction[0];
    directiony=direction[1];
    directionz=direction[2];
}

void SourceBase::makeOmniDirectional() {
    alSource3f(sourcename_,AL_DIRECTION,0.0,0.0,0.0);
}

void SourceBase::setSoundCone(float innerangle, float outerangle, float outergain) {
    alSourcef(sourcename_,AL_CONE_INNER_ANGLE,innerangle);
    alSourcef(sourcename_,AL_CONE_OUTER_ANGLE,outerangle);
    alSourcef(sourcename_,AL_CONE_OUTER_GAIN,outergain);
}

void SourceBase::getSoundCone(float &innerangle, float &outerangle, float &outergain) const {
    alGetSourcefv(sourcename_,AL_CONE_INNER_ANGLE,&innerangle);
    alGetSourcefv(sourcename_,AL_CONE_OUTER_ANGLE,&outerangle);
    alGetSourcefv(sourcename_,AL_CONE_OUTER_GAIN,&outergain);
}

void SourceBase::setGain(float gain) {
    alSourcef(sourcename_,AL_GAIN,gain);
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

float SourceBase::getGain() const {
    ALfloat gain;
    alGetSourcefv(sourcename_,AL_GAIN,&gain);
    return gain;
}

void SourceBase::setMinMaxGain(float min, float max) {
    alSourcef(sourcename_,AL_MIN_GAIN,min);
    alSourcef(sourcename_,AL_MAX_GAIN,max);
    ALenum error=alGetError();
    if(error!=AL_FALSE)
        switch(error) {
      case(AL_INVALID_VALUE): {
          std::ostringstream str;
          str << "Invalid value for min/max gain: " << alGetErrorString(error) << std::ends;
          throw ValueError(str.str().c_str());
                              }

                              break;
      default:
          {
              std::ostringstream str;
              str << "Invalid value for min/max gain: " << alGetErrorString(error) << std::ends;
              throw ValueError(str.str().c_str());
          }
    }
}

void SourceBase::getMinMaxGain(float & min, float &max) const {
    alGetSourcefv(sourcename_,AL_MIN_GAIN,&min);
    alGetSourcefv(sourcename_,AL_MAX_GAIN,&max);
}

void SourceBase::setAmbient(bool ambient) {
    if(ambient) {
        alSourcei(sourcename_,AL_SOURCE_RELATIVE,AL_TRUE);
        alSourcef(sourcename_,AL_ROLLOFF_FACTOR,0.0);
        // TODO: if AL_ROLLOFF_FACTOR doesn't work; set
        // AL_MAX_GAIN=AL_MIN_GAIN=AL_GAIN for this source..
        makeOmniDirectional();
    } else {
        alSourcei(sourcename_,AL_SOURCE_RELATIVE,AL_FALSE);
        alSourcef(sourcename_,AL_ROLLOFF_FACTOR,1.0);
    }
    int error = alGetError();
    if(error !=AL_FALSE)
    {
        std::ostringstream str;
        str << "Error trying to make/unmake sound ambient: " << alGetErrorString(error) << std::ends;
        throw ValueError(str.str().c_str());
    }

}

bool SourceBase::isAmbient() const {
    ALint relative;
    alGetSourceiv(sourcename_,AL_SOURCE_RELATIVE,&relative);
    ALfloat rolloff;
    alGetSourcefv(sourcename_,AL_ROLLOFF_FACTOR,&rolloff);
    return (relative==AL_TRUE && rolloff<0.001);
}

void SourceBase::setRelative(bool relative) {
    if(relative)
        alSourcei(sourcename_,AL_SOURCE_RELATIVE,AL_TRUE);
    else
        alSourcei(sourcename_,AL_SOURCE_RELATIVE,AL_FALSE);
}

bool SourceBase::isRelative() const {
    ALint relative;
    alGetSourceiv(sourcename_,AL_SOURCE_RELATIVE,&relative);
    return (relative==AL_TRUE);
}

void SourceBase::setReferenceDistance(float distance) {
    alSourcef(sourcename_,AL_REFERENCE_DISTANCE,distance);
    int error = alGetError();
    if(error !=AL_FALSE)
    {
        std::ostringstream str;
        str << "Error trying to set reference distance: " << alGetErrorString(error) << std::ends;
        throw FatalError(str.str().c_str());
    }
}

float SourceBase::getReferenceDistance() const {
    ALfloat ref;
    alGetSourcefv(sourcename_,AL_REFERENCE_DISTANCE,&ref);
    return ref;
}

void SourceBase::setMaxDistance(float distance) {
    alSourcef(sourcename_,AL_MAX_DISTANCE,distance);
    int error = alGetError();
    if(error!=AL_FALSE)
    {
        std::ostringstream str;
        str << "Error trying to set max distance: " << alGetErrorString(error) << std::ends;
        throw FatalError(str.str().c_str());
    }
}

float SourceBase::getMaxDistance() const {
    ALfloat maxdistance;
    alGetSourcefv(sourcename_,AL_MAX_DISTANCE,&maxdistance);
    return maxdistance;
}

void SourceBase::setRolloffFactor(float factor) {
    alSourcef(sourcename_,AL_ROLLOFF_FACTOR,factor);
    int error = alGetError();
    if(error!=AL_FALSE)
    {
        std::ostringstream str;
        str << "Error trying to set rolloff factor: " << alGetErrorString(error) << std::ends;
        throw FatalError(str.str().c_str());
    }
}

float SourceBase::getRolloffFactor() const {
    ALfloat factor;
    alGetSourcefv(sourcename_,AL_ROLLOFF_FACTOR,&factor);
    return factor;
}

void SourceBase::setPitch(float pitch) {
    alSourcef(sourcename_,AL_PITCH,pitch);
    ALenum error=alGetError();
    if(error!=AL_FALSE)
        switch(error) {
      case(AL_INVALID_VALUE):
          {
              std::ostringstream str;
              str << "Invalid value for pitch" << alGetErrorString(error) << std::ends;
              throw ValueError(str.str().c_str());
          }
          break;
      default:
          {
              std::ostringstream str;
              str << "Error trying to set pitch: " << alGetErrorString(error) << std::ends;
              throw FatalError(str.str().c_str());
          }
    }
}

float SourceBase::getPitch() const {
    ALfloat pitch;
    alGetSourcefv(sourcename_,AL_PITCH,&pitch);
    return pitch;
}

void SourceBase::setReverbScale(float scale) throw (InitError,ValueError) {
    if(reverbinitiated_) {
        alReverbScale(sourcename_,scale);
        int error = alGetError();
        if(error!=AL_FALSE) 
        {
            std::ostringstream str;
            str << "Reverb scale must be in range [0.0,1.0]" << alGetErrorString(error) << std::ends;
            throw ValueError(str.str().c_str());
        }
        reverbscale_=scale;
    } else
        throw InitError("Reverb not initialized");
}

void SourceBase::setReverbDelay(float delay) throw (InitError,ValueError) {
    if(reverbinitiated_) {
        alReverbDelay(sourcename_,delay);
        int error = alGetError();  
        if(error!=AL_FALSE)
        {
            std::ostringstream str;
            str << "Reverb delay must be in range [0.0,1.0]" << alGetErrorString(error) << std::ends;
            throw ValueError(str.str().c_str());
        }
        reverbdelay_=delay;
    } else
        throw InitError("Reverb not initialized");
}

float SourceBase::getReverbScale() throw (InitError) {
    if(!reverbinitiated_)
        throw InitError("Reverb not initialized");
    return reverbscale_;
}

float SourceBase::getReverbDelay() throw (InitError) {
    if(!reverbinitiated_)
        throw InitError("Reverb not initialized");
    return reverbdelay_;
}

ALuint SourceBase::link(const SourceBase *source) throw (MemoryError) {
    if(nlinkedsources_>=alloclinkedsources_) {
        alloclinkedsources_+=5;
        linkedsources_=(ALuint *)realloc(linkedsources_,
            sizeof(ALuint)*alloclinkedsources_);
        if(!linkedsources_)
            throw MemoryError("realloc failed");
    }
    linkedsources_[nlinkedsources_++]=source->getAlSource();
    return source->getAlSource();
}

void SourceBase::unlink(const SourceBase *source) throw (NameError) {
    if(source->sourcename_==sourcename_)
        throw(NameError("Can't unlink a source from itself!"));
    for(unsigned int i=0;i<nlinkedsources_;i++)
        if(linkedsources_[i]==source->sourcename_) {
            linkedsources_[i]=linkedsources_[--nlinkedsources_];
            return;
        }
        throw NameError("Source not linked");
}

void SourceBase::unlink(const ALuint name) throw (NameError) {
    if(name==sourcename_)
        throw(NameError("Can't unlink a source from itself!"));
    for(unsigned int i=0;i<nlinkedsources_;i++)
        if(linkedsources_[i]==name) {
            linkedsources_[i]=linkedsources_[--nlinkedsources_];
            return;
        }
        throw NameError("Source not linked");
}

void SourceBase::unlinkAll() {
    nlinkedsources_=1;
}

ALuint SourceBase::getAlSource() const {
    return sourcename_;
}

void SourceBase::setPosition(float x, float y, float z) {
    ALenum error=alGetError();
    alSource3f(sourcename_,AL_POSITION,x,y,z);
    error=alGetError();
    if(error!=AL_FALSE)
        switch(error) {
      case(AL_INVALID_VALUE):
          {
              std::ostringstream str;
              str << "Invalid value for position" << alGetErrorString(error) << std::ends;
              throw ValueError(str.str().c_str());
          }
          break;
      default:
          {
              std::ostringstream str;
              str << "Error trying to set position: " << alGetErrorString(error) << std::ends;
              throw FatalError(str.str().c_str());
          }
    }
}

void SourceBase::getPosition(float &x, float &y, float &z) const {
    ALfloat position[3];
    alGetSourcefv(sourcename_,AL_POSITION,position);
    x=position[0];
    y=position[1];
    z=position[2];
}

void SourceBase::setVelocity(float vx, float vy, float vz) {
    alSource3f(sourcename_,AL_VELOCITY,vx,vy,vz);
    ALenum error=alGetError();
    if(error!=AL_FALSE)
    {
        std::ostringstream str;
        str << "Error setting velocity for reverb: " << alGetErrorString(error) << std::ends;
        throw ValueError(str.str().c_str());
    }
}

void SourceBase::getVelocity(float &vx, float &vy, float &vz) const {
    ALfloat velocity[3];
    alGetSourcefv(sourcename_,AL_VELOCITY,velocity);
    vx=velocity[0];
    vy=velocity[1];
    vz=velocity[2];
}

