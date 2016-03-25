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

#include <osgAudio/AudioEnvironment.h>
#include <openalpp/AudioEnvironment.h>

using namespace osgAudio;


/**
* Used when translating the distance model between osgAudio and openAL++
*/
static openalpp::DistanceModel _openalppDistanceModelTranslate[] = {
    openalpp::None,openalpp::InverseDistance,openalpp::InverseDistanceClamped,openalpp::Linear
    };
static osgAudio::DistanceModel _osgAudioDistanceModelTranslate[] = {
    None,InverseDistance,InverseDistanceClamped,Linear
};

AudioEnvironment::AudioEnvironment()
: _dummyUnitScale(1.0f)
{
}

AudioEnvironment* AudioEnvironment::instance()
{
    static AudioEnvironment* s_audioEnvironment = new AudioEnvironment();
    return s_audioEnvironment;
}

void AudioEnvironment::init( bool displayInitMsgs ) throw (InitError) 
{
    try 
    {
        _openalppAudioEnvironment = new openalpp::AudioEnvironment( displayInitMsgs);
    }
    catch(openalpp::InitError error)
    {
        throw InitError(error.what());
    }
}

void AudioEnvironment::init(int frequency,int refresh,bool synchronous,bool displayInitMsgs)
throw (InitError) 
{
    try 
    {
        _openalppAudioEnvironment = new openalpp::AudioEnvironment (frequency, refresh, synchronous, displayInitMsgs);
    }
    catch(openalpp::InitError error)
    { 
        throw InitError(error.what());
    }
}

void AudioEnvironment::init(int frequency,int refresh) throw (InitError)
{
    try 
    {
        _openalppAudioEnvironment = new openalpp::AudioEnvironment (frequency, refresh);
    }
    catch(openalpp::InitError error)
    {
        throw InitError(error.what());
    }
}

AudioEnvironment::~AudioEnvironment()
{
    if(_openalppAudioEnvironment.valid())
    {
        _openalppAudioEnvironment = NULL; // deletes it
    } // if
} // AudioEnvironment::~AudioEnvironment

void AudioEnvironment::shutdown()
{
    if(_openalppAudioEnvironment.valid())
    {
        _openalppAudioEnvironment = NULL; // deletes it
    }
}

void AudioEnvironment::setSoundVelocity(float speed) throw(ValueError,FatalError){
    try {
    _openalppAudioEnvironment->setSoundVelocity (speed);
    }
    catch(openalpp::ValueError error) { throw ValueError(error.what()); }
    catch(openalpp::FatalError error) { throw FatalError(error.what()); }
}

float AudioEnvironment::getSoundVelocity() throw (FatalError) {
    float speed;
    try {
    speed = _openalppAudioEnvironment->getSoundVelocity();
    }
    catch(openalpp::FatalError error) { throw FatalError(error.what()); }
    return speed;
}

void AudioEnvironment::setDopplerFactor(float factor) 
throw (ValueError,FatalError) {
    try {
    _openalppAudioEnvironment->setDopplerFactor (factor);
    }
    catch(openalpp::ValueError error) { throw ValueError(error.what()); }
    catch(openalpp::FatalError error) { throw FatalError(error.what()); }
}

float AudioEnvironment::getDopplerFactor() throw (FatalError) {
    float factor;
    try {
    factor = _openalppAudioEnvironment->getDopplerFactor();
    }
    catch(openalpp::FatalError error) { throw FatalError(error.what()); }
    return factor;
}

void AudioEnvironment::setUnitScale(float scale) throw (ValueError,FatalError) {
    _dummyUnitScale = scale;
} 

float AudioEnvironment::getUnitScale() throw (FatalError) {
    return(_dummyUnitScale);
}

void AudioEnvironment::setGain(float gain) {
    try {
    _openalppAudioEnvironment->setGain (gain);
    }
    catch(openalpp::ValueError error) { throw ValueError(error.what()); }
    catch(openalpp::FatalError error) { throw FatalError(error.what()); }
}

float AudioEnvironment::getGain() throw (FatalError) {  
    float gain;
    try {
    gain = _openalppAudioEnvironment->getGain();
    }
    catch(openalpp::FatalError error) { throw FatalError(error.what()); }
    return gain;
}

void AudioEnvironment::setDistanceModel(DistanceModel model)
throw (FatalError){
    try {
    _openalppAudioEnvironment->setDistanceModel (_openalppDistanceModelTranslate[model]);
    }
    catch(openalpp::FatalError error) { throw FatalError(error.what()); }
}

DistanceModel AudioEnvironment::getDistanceModel() throw (FatalError) {
    DistanceModel dmodel;
    try {
    dmodel = _osgAudioDistanceModelTranslate[_openalppAudioEnvironment->getDistanceModel()];
    }
    catch(openalpp::FatalError error) { throw FatalError(error.what()); }
    return dmodel;
}


void AudioEnvironment::initiateReverb() throw (InitError) {
    try {
    _openalppAudioEnvironment->initiateReverb();
    }
    catch(openalpp::InitError error) { throw InitError(error.what()); }
}

