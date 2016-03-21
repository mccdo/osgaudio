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

#include <osgAudio/AudioEnvironment.h>
#include <cassert>


using namespace osgAudio;


/**
* Used when translating the distance model between osgAudio and openAL++
*
* FMOD doesn't seem to offer equivaents to None or unclamped Log. It does
* offer set3DCustomRolloff() but that seems excessive for this use and is
* therefore currently unimplemented.
*/
#if FMOD_VERSION < 0x00043405
static FMOD_MODE _FMODDistanceModelTranslate[] = {
	FMOD_3D_LOGROLLOFF,FMOD_3D_LOGROLLOFF,FMOD_3D_LOGROLLOFF,FMOD_3D_LINEARROLLOFF
	};
#else
static FMOD_MODE _FMODDistanceModelTranslate[] = {
	FMOD_3D_INVERSEROLLOFF,FMOD_3D_INVERSEROLLOFF,FMOD_3D_INVERSEROLLOFF,FMOD_3D_LINEARROLLOFF
	};
#endif

AudioEnvironment::AudioEnvironment()
    :
    _system( 0 ),
    _preSystemInitFunc( 0 )
{
    ;
}

AudioEnvironment* AudioEnvironment::instance()
{
	static AudioEnvironment* s_audioEnvironment = new AudioEnvironment();
	return s_audioEnvironment;
}

void AudioEnvironment::init( bool displayInitMsgs ) throw (InitError)
{
	initInternals();

    // initializes System singleton if needed
	getSystem( displayInitMsgs ); 
}

void AudioEnvironment::init(int frequency,int refresh,bool synchronous, bool displayInitMsgs) throw (InitError)
{
    initInternals();

    // initializes System singleton if needed
	getSystem( displayInitMsgs ); 
} 

void AudioEnvironment::init(int frequency,int refresh, bool displayInitMsgs) throw (InitError)
{
	initInternals();

    // initializes System singleton if needed
	getSystem( displayInitMsgs ); 
} // AudioEnvironment::AudioEnvironment

void AudioEnvironment::initInternals()
{
	//_dopplerFactor = 1.0;  // re-inited by calcFMODDopplerFactor, below
	_rolloffScale = 1.0;
	_unitScale = 1.0; // 1.0 = meters;
	_gain = 1.0;

#if FMOD_VERSION < 0x00043405
	_internalDistanceModel = FMOD_3D_LOGROLLOFF;
#else
	_internalDistanceModel = FMOD_3D_INVERSEROLLOFF;
#endif
	_externalDistanceModel = InverseDistanceClamped;

	_speedOfSound = 343.0f;
	_dopplerFactor = calcFMODDopplerFactor(_speedOfSound);

} // AudioEnvironment::initInternals

AudioEnvironment::~AudioEnvironment()
{
    _preSystemInitFunc = 0;
	// <<<>>> FMOD doesn't really support any concept of multiple discreet systems, so we
	// likewise can't really support multiple discrete AudioEnvironments. Therefore
	// reference countin the _system doesn't really benefit us. Just release it when
	// we're done. If we want to support multiple AudioEnvironments, we will need to
	// reference count the _system singleton
	if(_system)
	{
        _system->close();

		_system->release();
		_system = NULL;
	} // if
} // AudioEnvironment::~AudioEnvironment

float AudioEnvironment::calcFMODDopplerFactor(float speed) const 
{

	// FMOD does not support user-configured speed of sound exactly
	//   "Actually the [doppler scale] setting in System::set3DSettings is only used
	//   for doppler calculation in the fashion SPEED_OF_SOUND * [doppler scale]. We
	//   have SPEED_OF_SOUND hardcoded to 340, so you can use the [doppler scale] to
	//   map our speed of sound to the speed of sound you require."
	//   http://www.fmod.org/forum/viewtopic.php?p=44648#44648
	// [] are corrections made to the original posting, for clarification
	// So, we calculate _dopplerFactor from speed and use that.
	return speed / 343.0; // OpenAL defaulted to 343, not 340
} // AudioEnvironment::calcFMODDopplerFactor

void AudioEnvironment::setSoundVelocity(float speed) throw(ValueError,FatalError)
{
	FMOD_RESULT result;

	// calculate _dopplerFactor from speed and use that.
	_dopplerFactor = calcFMODDopplerFactor(speed);
	result = _system->set3DSettings(_dopplerFactor, _unitScale, _rolloffScale);
	if(result != FMOD_OK)
	{
		throw ValueError("FMOD set3DSettings() in setSoundVelocity");
		// FMOD doesn't throw anything like FatalError
	} // if
} // AudioEnvironment::setSoundVelocity

float AudioEnvironment::getSoundVelocity() throw (FatalError) 
{
	return _dopplerFactor;
} // AudioEnvironment::getSoundVelocity

void AudioEnvironment::setDopplerFactor(float factor) 
throw (ValueError,FatalError) 
{
	FMOD_RESULT result;

	_dopplerFactor = factor;
	result = _system->set3DSettings(_dopplerFactor, _unitScale, _rolloffScale);
	if(result != FMOD_OK)
	{
		throw ValueError("FMOD set3DSettings() in setDopplerFactor");
		// FMOD doesn't throw anything like FatalError
	} // if
} // AudioEnvironment::setDopplerFactor

float AudioEnvironment::getDopplerFactor() throw (FatalError) 
{
	return _dopplerFactor;
	// FMOD doesn't throw anything like FatalError
} // AudioEnvironment::getDopplerFactor

void AudioEnvironment::setUnitScale(float scale) throw (ValueError,FatalError) {
	_unitScale = scale;
	_system->set3DSettings(_dopplerFactor, _unitScale, _rolloffScale);
} 

float AudioEnvironment::getUnitScale() throw (FatalError) {
	return(_unitScale);
}

void AudioEnvironment::setGain(float gain) {
	FMOD::ChannelGroup *channelgroup;
	_gain = gain;
	_system->getMasterChannelGroup(&channelgroup);
	channelgroup->setVolume(_gain);
} // AudioEnvironment::setGain

float AudioEnvironment::getGain() throw (FatalError) 
{  
	// FMOD doesn't throw anything like FatalError
	return _gain;
} // AudioEnvironment::getGain

void AudioEnvironment::setDistanceModel(DistanceModel model) throw (FatalError) 
{
	//FMOD_RESULT result;

	_externalDistanceModel = model;

	// FMOD seems to set this on a per-Channel or per-sound basis with 
	// Sound::setMode or Channel::setMode, but I haven't found a global default
	// that can be changed after System creation-time.
	_internalDistanceModel = _FMODDistanceModelTranslate[model];
	// FMOD sets this on each Source channel as it is created
} // AudioEnvironment::setDistanceModel

DistanceModel AudioEnvironment::getDistanceModel() throw (FatalError) 
{
	return _externalDistanceModel;
} // AudioEnvironment::getDistanceModel

void AudioEnvironment::initiateReverb() throw (InitError) 
{
    ;// apparenlty un-needed in FMOD
} // AudioEnvironment::initiateReverb

void AudioEnvironment::setPreSystemInitFunc( void (*preSystemInitFunc)(FMOD::System*) )
{
    _preSystemInitFunc = preSystemInitFunc;
}

FMOD::System *AudioEnvironment::getSystem( bool displayInitMsgs ) throw (InitError)
{
	// <<<>>> need some thread-singleton blocking here
	if(!_system)
	{
		FMOD_RESULT result;

		result = FMOD::System_Create(&_system);
		if(result != FMOD_OK)
		{
			throw InitError("Unable to create FMOD::System.");
		} // if
        
        //Call a users function to initialize system before the init call 
        //is made. This can be used to tell FMOD which FMOD driver to use
        //and what FMOD speaker configuration to use.
        if( _preSystemInitFunc != 0 )
        {
            _preSystemInitFunc( _system );
        }

        //FMOD doesn't share the same semantics as OpenAL -- you cannot 
        //configure settings like volume gain on a sound that is not currently 
        //playing. OpenAL (and therefore osgAL) allowed you to pre-configre 
        //things like volume gain prior to setting the sound playing.
        
        //To emulate this, all sounds in FMOD are created as already playing, 
        //but paused at the beginning. In this state, their settings like volume 
        //gain can be adjusted and the adjustments will be retained and adhered 
        //to when unpaused. Telling a sound to play is really telling it 
        //to un-pause.
        
        //Therefore, every sound that exists in FMOD is technically "playing", 
        //but paused, and consuming some resources.
        
        //FMOD normally exposes 32 hardware playback channels, and another 32 
        //software playback channels that are combined together and fed to the 
        //hardware as one channel. This is the 64-sound limit Ross discovered. 
        //The 65th sound simply had no resources to play through. I increased 
        //the pre-set number of software channels to 100.
        
        // Setup 100 virtual channels
		result = _system->setSoftwareChannels(100);	
        if(result != FMOD_OK)
		{
			throw InitError("Unable to the number of software channels.");
		} // if
        
        // Initialize FMOD.
		result = _system->init(100, FMOD_INIT_3D_RIGHTHANDED, 0);	
        if(result != FMOD_OK)
		{
			throw InitError("Unable to init the FMOD System.");
		} // if
        
        if( displayInitMsgs )
        {
            int numDrivers;
            result = _system->getNumDrivers( &numDrivers );

            std::cout << "Initializing FMOD. Detected " << numDrivers << " drivers." << std::endl;

            int idx;
            for( idx=0; idx<numDrivers; idx++ )
            {
                const int nameLen( 128 );
                char name[ nameLen ];
                FMOD_GUID guid;
                result = _system->getDriverInfo( idx, name, nameLen, &guid );
                std::cout << idx << ": " << name << std::endl;
            }
        }
	} // if
	return _system;
} // AudioEnvironment::getSystem

void AudioEnvironment::update() 
{
	getSystem()->update(); // Makes the audio world go round. Literally.
} // AudioEnvironment::getSystem
