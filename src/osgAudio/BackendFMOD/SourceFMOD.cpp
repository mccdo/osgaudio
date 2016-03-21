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

#include <cassert>
#include <limits>

#include <osgAudio/Source.h>
#include <osgAudio/Sample.h>
#include <osgAudio/FileStream.h>
#include <osgAudio/AudioEnvironment.h>
#include <osgAudio/SoundDefaults.h>

using namespace osgAudio;


Source::Source(float x, float y, float z) {
	internalInit();

	_position.x = x;
	_position.y = y;
	_position.z = z;
}

Source::Source(const std::string& filename) {
	internalInit();

	setSound(new Sample(filename));
}


Source::Source( Stream *stream) {
	internalInit();

	setSound(new Stream(*stream));
}

Source::Source( Sample *sample) {
	internalInit();

	setSound(new Sample(sample->getFilename()));
}

Source::Source( const Source &source) {
	internalInit();

	// FMOD doesn't offer an easy way to clone a sound, so we
	// copy a reference to it, and rely on osg's ref counting
	// but the internal Sound object is still a single, shared instance
	_Sound = source._Sound;
	if(_FMODChannel)
	{
		_FMODChannel->stop(); // release channel
		_FMODChannel = NULL;
	} // if
	// start it, paused, so we can tinker with the settings if needed before playing
	FMOD_RESULT createResult = osgAudio::AudioEnvironment::instance()->getSystem()->playSound(FMOD_CHANNEL_FREE, _Sound->getInternalSound(), true, &_FMODChannel);
	initPitchSupport();

}

void Source::internalInit(void)
{
	_FMODChannel = NULL;
	_Sound = NULL;
	_position.x = _position.y = _position.z = 0.0f;
	_velocity.x = _velocity.y = _velocity.z = 0.0f;
	_zero.x = _zero.y = _zero.z = 0.0f;
	_maxDistance = _init_maxDistance();
	_minDistance = _init_referenceDistance();
	_volume = _init_gain();

	_isAmbient = _isRelative = false;

	// These two parameters are unimplemented in FMOD
	_reverbScale = _init_reverbScale();
	_reverbDelay = _init_reverbDelay();

	_rolloffFactor = 0.0; // FMOD doesn't currently support this kind of factor
	_pitch = _init_pitch();
	_nominalFrequency = 0.0; // undefined until playing

	// cone settings
	_innerAngle = _init_innerAngle();
	_outerAngle = _init_outerAngle();
	_outerGain  = _init_outerGain();

} // Source::internalInit


Source::~Source() {
// stop the source so that the channel can be released
	if(_FMODChannel)
	{
		_FMODChannel->stop(); // release channel
	} // if
}

// TODO: Add flag for whether the sound should be loaded. This is useful for
// dummy sources that will be used only for groupsources.
void Source::setSound(const std::string& filename) {
	_Sound = new FileStream(filename);
	if(_FMODChannel)
	{
		_FMODChannel->stop(); // release channel
		_FMODChannel = NULL;
	} // if
	// start it, paused, so we can tinker with the settings if needed before playing
	FMOD_RESULT createResult = osgAudio::AudioEnvironment::instance()->getSystem()->playSound(FMOD_CHANNEL_FREE, _Sound->getInternalSound(), true, &_FMODChannel);
	initPitchSupport();
}

void Source::setSound( Sample *buffer ) {
	_Sound = buffer;
	if(_FMODChannel)
	{
		_FMODChannel->stop(); // release channel
		_FMODChannel = NULL;
	} // if
	// start it, paused, so we can tinker with the settings if needed before playing
	FMOD_RESULT createResult = osgAudio::AudioEnvironment::instance()->getSystem()->playSound(FMOD_CHANNEL_FREE, _Sound->getInternalSound(), true, &_FMODChannel);
	initPitchSupport();
}

void Source::setSound(Stream *stream) {
	_Sound = stream;
	if(_FMODChannel)
	{
		_FMODChannel->stop(); // release channel
		_FMODChannel = NULL;
	} // if
	// start it, paused, so we can tinker with the settings if needed before playing
	FMOD_RESULT createResult = osgAudio::AudioEnvironment::instance()->getSystem()->playSound(FMOD_CHANNEL_FREE, _Sound->getInternalSound(), true, &_FMODChannel);
	initPitchSupport();
}

const SoundData *Source::getSound() const {
	assert(0); // <<<>>> unimplemented
	return(NULL);
}

void Source::play(const std::string& filename) {
	setSound(filename);
	play();
}

void Source::play( Sample *buffer) {
	setSound(buffer);
	play();
}

void Source::play( Stream *stream ) {
	setSound(stream);
	play();
}

void Source::play() {
	bool isPlaying;
	if(_FMODChannel)
	{
		_FMODChannel->isPlaying(&isPlaying);
		if(isPlaying) // already playing
		{
			bool paused;
			_FMODChannel->getPaused(&paused);
			if(paused) // is it paused?
			{
				_FMODChannel->setPaused(false); // unpause
			} // if
			return;
		} // if
	} // if
	// not yet playing
	FMOD_RESULT createResult = osgAudio::AudioEnvironment::instance()->getSystem()->playSound(FMOD_CHANNEL_FREE, _Sound->getInternalSound(), false, &_FMODChannel);
	initPitchSupport();
}

void Source::seek(float time_s)
{
	if(_FMODChannel)
	{
        unsigned int pos = time_s * 1000;
		_FMODChannel->setPosition( pos, FMOD_TIMEUNIT_MS);
	} // if
}

void Source::stop() {
	// here, paused is same as stopped because stopped may release the Channel ID
	if(_FMODChannel)
	{
		_FMODChannel->setPaused(true);
	} // if
}

void Source::pause() {
	if(_FMODChannel)
	{
		_FMODChannel->setPaused(true);
	} // if
}


bool Source::isStreaming() {
	osgAudio::Stream *testStream = dynamic_cast<osgAudio::Stream *>(_Sound.get());
	return(testStream != NULL);
}

Source &Source::operator=(const Source &source) {
	if(this!=&source) {
	assert(0); // <<<>>> unimplemented
	}
	return *this;
}

// methods inherited from SourceBase

void Source::rewind() {
	if(_FMODChannel)
	{
		_FMODChannel->setPosition(0, FMOD_TIMEUNIT_MS);
	} // if
}

SourceState Source::getState() const {

	bool testState;
	if(_FMODChannel)
	{
		_FMODChannel->isPlaying(&testState);
		if(testState)
		{
			if(isPaused())
			{
			return Paused;
			} // if
			else
			{
			return Playing;
			} // else
		} // if
		else
		{
			return Stopped;
		} // else
	} // if
	return Initial;
}

void Source::setLooping(bool loop) {
	if(_FMODChannel)
	{
		if(loop)
		{
			_FMODChannel->setLoopCount(-1);
			_FMODChannel->setMode(FMOD_LOOP_NORMAL);
		} // if
		else
		{
			_FMODChannel->setLoopCount(0);
			_FMODChannel->setMode(FMOD_LOOP_OFF);
		} // else
	} // if
}

bool Source::isLooping() const {
	if(_FMODChannel)
	{
		int loopCount;
		_FMODChannel->getLoopCount(&loopCount);
		return(loopCount == -1);
	} // if
return(false);
}

bool Source::isPaused() const {
	if(_FMODChannel)
	{
		bool isPaused;
		_FMODChannel->getPaused(&isPaused);
		return(isPaused);
	} // if
return true; // if we have no channel, it certainly can't be said to be "playing"
}

void Source::setDirection(float directionx, float directiony, float directionz) {
	_direction.x = directionx;
	_direction.y = directiony;
	_direction.z = directionz;
	if(_FMODChannel)
	{
		_FMODChannel->set3DConeOrientation(&_direction);
	} // if
}

void Source::getDirection(float &directionx, float &directiony, float &directionz) const {
	directionx = _direction.x;
	directiony = _direction.y;
	directionz = _direction.z;
}

void Source::makeOmniDirectional() {
	if(_FMODChannel)
	{
		_FMODChannel->set3DConeSettings(360, 360, 1.0);
	} // if
}

void Source::setSoundCone(float innerangle, float outerangle, float outergain) {
	_innerAngle = innerangle;
	_outerAngle = outerangle;
	_outerGain = outergain;
	if(_FMODChannel)
	{
		_FMODChannel->set3DConeSettings(innerangle, outerangle, outergain);
	} // if
}

void Source::getSoundCone(float &innerangle, float &outerangle, float &outergain) const {
	innerangle = _innerAngle;
	outerangle = _outerAngle;
	outergain = _outerGain;
}

void Source::setGain(float gain) {
	_volume = gain;
	if(_FMODChannel)
	{
		_FMODChannel->setVolume(_volume);
	} // if
}

float Source::getGain() const {
	return _volume;
}

void Source::setMinMaxGain(float min, float max) {
	assert(0); // <<<>>> unimplemented in FMOD per http://www.fmod.org/forum/viewtopic.php?p=44660#44660
}

void Source::getMinMaxGain(float & min, float &max) const {
	assert(0); // <<<>>> unimplemented in FMOD per http://www.fmod.org/forum/viewtopic.php?p=44660#44660
}

void Source::setAmbient(bool ambient) {
	_isAmbient = ambient;
	if(_FMODChannel)
	{
		updateRelativeAmbientState();
	} // if
}

bool Source::isAmbient() const {
	return _isAmbient;
}

void Source::setRelative(bool relative) {
	_isRelative = relative;
	if(_FMODChannel)
	{
		updateRelativeAmbientState();
	} // if
}

bool Source::isRelative() const {
	return _isRelative;
}

void Source::setReferenceDistance(float distance) {
	_minDistance = distance;
	if(_FMODChannel)
	{
		// FMOD's MinDistance seems equivalent to OpenAL's "reference" distance
		// OpenAL's Inverse-Distance-Clamped/IASIG I3DL2 says
		// "guarantee that for distances below AL_REFERENCE_DISTANCE, gain is clamped."
		// which sounds to me like FMOD MinDistance
		// "'mindistance' of a sound / channel, is the distance that the sound starts to attenuate from. "
		_FMODChannel->set3DMinMaxDistance(_minDistance, _maxDistance);
	} // if
}

float Source::getReferenceDistance() const {
	return _minDistance;
}

void Source::setMaxDistance(float distance) {
	_maxDistance = distance;
	if(_FMODChannel)
	{
		_FMODChannel->set3DMinMaxDistance(_minDistance, _maxDistance);
	} // if
}

float Source::getMaxDistance() const {
	return _maxDistance;
}

void Source::setRolloffFactor(float factor) {
	// <<<>>> FMOD only supports custom rolloff factors on a systemwide basis
	// unless you use a FMOD_3D_CUSTOMROLLOFF
	// "This rolloff rate can be changed with System::set3DSettings."
}

float Source::getRolloffFactor() const {

	FMOD_REVERB_CHANNELPROPERTIES prop;
	prop.Flags = FMOD_REVERB_CHANNELFLAGS_DEFAULT;

	if(_FMODChannel)
	{
		_FMODChannel->getReverbProperties(&prop);
	} // if

#if( FMOD_VERSION >= 0x00043600 )
    // RolloffFactor no longer a memver of FMOD_REVERB_CHANNELPROPERTIES?!?
    return( 1.f );
#else
	return(prop.RolloffFactor);
#endif
}

void Source::initPitchSupport(void)
{
	if(_FMODChannel)
	{
		// fetch nominal frequency for later fiddling if desired
		_FMODChannel->getFrequency(&_nominalFrequency);

		// setup modified frequency if needed
		updateModifiedFrequency();

	} // if
} // Source::initPitchSupport


void Source::updateModifiedFrequency(void)
{
	_FMODChannel->setFrequency(_nominalFrequency * _pitch);
} // Source::updateModifiedFrequency


void Source::setPitch(float pitch) {
	_pitch = pitch;
	updateModifiedFrequency();
}

float Source::getPitch() const {
	return(_pitch);
}

void Source::setReverbScale(float scale) throw (InitError,ValueError) {
	_reverbScale = scale;
	// The exact meaning of the Reverb Scale and Delay parameters is only
	// understood within the context of the alReverbScale_LOKI/alReverbDelay_LOKI
	// extensions, which appear to no longer be documented. Implementing this
	// capability via FMOD seems improbable until it is understood.
	if(_FMODChannel)
	{
		assert(0); // <<<>>> unimplemented
		//_FMODChannel->
	} // if
}

void Source::setReverbDelay(float delay) throw (InitError,ValueError) {
	_reverbDelay = delay;
	// The exact meaning of the Reverb Scale and Delay parameters is only
	// understood within the context of the alReverbScale_LOKI/alReverbDelay_LOKI
	// extensions, which appear to no longer be documented. implementing this
	// capability via FMOD seems improbable until it is understood.
	if(_FMODChannel)
	{
		assert(0); // <<<>>> unimplemented
		//_FMODChannel->
	} // if
}

float Source::getReverbScale() throw (InitError) {
	return(_reverbScale);
}

float Source::getReverbDelay() throw (InitError) {
	return(_reverbDelay);
}

// <<<>>> May need to re-engineer
/*
unsigned int Source::link(const SourceBase *source) throw (MemoryError) {
	try {
	return _openalppSource->link(source);
	}
	catch(openalpp::MemoryError error) { throw MemoryError(error.what()); }
}

void Source::unlink(const SourceBase *source) throw (NameError) {
	try {
	return _openalppSource->link(source);
	}
	catch(openalpp::MemoryError error) { throw MemoryError(error.what()); }
}

void Source::unlink(const unsigned int name) throw (NameError) {
	try {
	_openalppSource->unlink(name);
	}
	catch(openalpp::NameError error) { throw NameError(error.what()); }
}

void Source::unlinkAll() {
	_openalppSource->unlinkAll();
}


unsigned int Source::getAlSource() const {
	return _openalppSource->getAlSource();
}

*/

void Source::setPosition(float x, float y, float z) {
	_position.x = x;
	_position.y = y;
	_position.z = z;
	updatePositionVelocity3d();
}

void Source::getPosition(float &x, float &y, float &z) const {
	x = _position.x;
	y = _position.y;
	z = _position.z;
}

void Source::setVelocity(float vx, float vy, float vz) {
	_velocity.x = vx;
	_velocity.y = vy;
	_velocity.z = vz;
	updatePositionVelocity3d();
}

void Source::getVelocity(float &vx, float &vy, float &vz) const {
	vx = _velocity.x;
	vy = _velocity.y;
	vz = _velocity.z;
}

void Source::updatePositionVelocity3d(void)
{
	if(_FMODChannel)
	{
		if(_isAmbient)
		{
			_FMODChannel->set3DAttributes(&_zero, &_zero);
		} // if
		else
		{
			_FMODChannel->set3DAttributes(&_position, &_velocity);
		} // else
	} // if

} // Source::updatePositionVelocity3d

void Source::updateRelativeAmbientState(void)
{
	if(_FMODChannel)
	{
		_FMODChannel->setMode(_isRelative || _isAmbient ? FMOD_3D_HEADRELATIVE : FMOD_3D_WORLDRELATIVE);
		updatePositionVelocity3d(); // to set position to 0,0,0 for ambient or actual coordinates otherwise
	} // if

} // Source::updateRelativeAmbientState


FMOD::Channel *Source::getInternalSource(void) {
 return(_FMODChannel);
}

const FMOD::Channel *Source::getInternalSource(void) const {
 return(_FMODChannel);
}
