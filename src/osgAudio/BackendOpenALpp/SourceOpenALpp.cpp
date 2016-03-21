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

#include <osgAudio/Source.h>
#include <osgAudio/Sample.h>
#include <osgAudio/Stream.h>

#include <openalpp/Source.h>

using namespace osgAudio;


/**
* Used when translating the SourceState between osgAudio and openAL++
*/
static openalpp::SourceState _openalppSourceStateTranslate[] = {
    openalpp::Initial,
    openalpp::Playing,
    openalpp::Paused,
    openalpp::Stopped
    };
static osgAudio::SourceState _osgAudioSourceStateTranslate[] = {
    Initial,
    Playing,
    Paused,
    Stopped
};



Source::Source(float x, float y, float z) {
    try {
    _openalppSource = new openalpp::Source(x, y, z);
    }
    catch(openalpp::MemoryError error) { throw MemoryError(error.what()); }
    catch(openalpp::NameError error) { throw NameError(error.what()); }
}

Source::Source(const std::string& filename) {
    try {
    _openalppSource = new openalpp::Source();
    setSound(new Sample(filename));
    }
    catch(openalpp::MemoryError error) { throw MemoryError(error.what()); }
    catch(openalpp::NameError error) { throw NameError(error.what()); }
}


Source::Source( Stream *stream) {
    try {
    _openalppSource = new openalpp::Source(stream->getInternalStream());
    }
    catch(openalpp::MemoryError error) { throw MemoryError(error.what()); }
    catch(openalpp::NameError error) { throw NameError(error.what()); }
}

Source::Source( Sample *sample) {
    try {
    _openalppSource = new openalpp::Source(sample->getInternalSample());
    }
    catch(openalpp::MemoryError error) { throw MemoryError(error.what()); }
    catch(openalpp::NameError error) { throw NameError(error.what()); }
}

Source::Source( const Source &source) {
    try {
    _openalppSource = new openalpp::Source(*(source.getInternalSource()));
    }
    catch(openalpp::MemoryError error) { throw MemoryError(error.what()); }
    catch(openalpp::NameError error) { throw NameError(error.what()); }
}

Source::~Source() {
// nothing happens here, our wrapped class does it all
}

// TODO: Add flag for whether the sound should be loaded. This is useful for
// dummy sources that will be used only for groupsources.
void Source::setSound(const std::string& filename) {
    _openalppSource->setSound(filename);
}

void Source::setSound( Sample *buffer ) {
    _openalppSource->setSound(buffer->getInternalSample()); // need access to internal openAL version of Sample
}

void Source::setSound(Stream *stream) {
    _openalppSource->setSound(stream->getInternalStream()); // need access to internal openAL version of Stream
}

const SoundData *Source::getSound() const {
    //return _openalppSource->getSound();
    return(NULL); // we can't go from the openalpp::SoundData to a osgAudio::SoundData right now
}

void Source::play(const std::string& filename) {
    _openalppSource->play(filename);
}

void Source::play( Sample *buffer) {
    _openalppSource->play(buffer->getInternalSample()); // need access to internal openAL version of Sample
}

void Source::play( Stream *stream ) {
    _openalppSource->play(stream->getInternalStream()); // need access to internal openAL version of Stream
}

void Source::play() {
    _openalppSource->play();
}

void Source::seek(float time_s)
{
    _openalppSource->seek(time_s);
}

void Source::stop() {
    _openalppSource->stop();
}

void Source::pause() {
    _openalppSource->pause();
}


bool Source::isStreaming() {
    return     _openalppSource->isStreaming();
}

Source &Source::operator=(const Source &source) {
    if(this!=&source) {
    *_openalppSource = *(source._openalppSource.get());
    }
    return *this;
}

// methods inherited from SourceBase

void Source::rewind() {
    _openalppSource->rewind();
}

SourceState Source::getState() const {
    try {
    return _osgAudioSourceStateTranslate[_openalppSource->getState()];
    }
    catch(openalpp::FatalError error) { throw FatalError(error.what()); }
}

void Source::setLooping(bool loop) {
    _openalppSource->setLooping(loop);
}

bool Source::isLooping() const {
    return _openalppSource->isLooping();
}

bool Source::isPaused() const {
    return _openalppSource->isPaused();
}

void Source::setDirection(float directionx, float directiony, float directionz) {
    _openalppSource->setDirection(directionx, directiony, directionz);
}

void Source::getDirection(float &directionx, float &directiony, float &directionz) const {
    _openalppSource->getDirection(directionx, directiony, directionz);
}

void Source::makeOmniDirectional() {
    _openalppSource->makeOmniDirectional();
}

void Source::setSoundCone(float innerangle, float outerangle, float outergain) {
    _openalppSource->setSoundCone(innerangle, outerangle, outergain);
}

void Source::getSoundCone(float &innerangle, float &outerangle, float &outergain) const {
    _openalppSource->getSoundCone(innerangle, outerangle, outergain);
}

void Source::setGain(float gain) {
    try {
    _openalppSource->setGain(gain);
    }
    catch(openalpp::ValueError error) { throw ValueError(error.what()); }
    catch(openalpp::FatalError error) { throw FatalError(error.what()); }
}

float Source::getGain() const {
    return _openalppSource->getGain();
}

void Source::setMinMaxGain(float min, float max) {
    try {
    _openalppSource->setMinMaxGain(min, max);
    }
    catch(openalpp::ValueError error) { throw ValueError(error.what()); }
}

void Source::getMinMaxGain(float & min, float &max) const {
    _openalppSource->getMinMaxGain(min, max);
}

void Source::setAmbient(bool ambient) {
    try {
    _openalppSource->setAmbient(ambient);
    }
    catch(openalpp::ValueError error) { throw ValueError(error.what()); }
}

bool Source::isAmbient() const {
    return _openalppSource->isAmbient();
}

void Source::setRelative(bool relative) {
    _openalppSource->setRelative(relative);
}

bool Source::isRelative() const {
    return _openalppSource->isRelative();
}

void Source::setReferenceDistance(float distance) {
    try {
    _openalppSource->setReferenceDistance(distance);
    }
    catch(openalpp::FatalError error) { throw FatalError(error.what()); }
}

float Source::getReferenceDistance() const {
    return _openalppSource->getReferenceDistance();
}

void Source::setMaxDistance(float distance) {
    try {
    _openalppSource->setMaxDistance(distance);
    }
    catch(openalpp::FatalError error) { throw FatalError(error.what()); }
}

float Source::getMaxDistance() const {
    return _openalppSource->getMaxDistance();
}

void Source::setRolloffFactor(float factor) {
    try {
    _openalppSource->setRolloffFactor(factor);
    }
    catch(openalpp::FatalError error) { throw FatalError(error.what()); }
}

float Source::getRolloffFactor() const {
    return _openalppSource->getRolloffFactor();
}

void Source::setPitch(float pitch) {
    try {
    _openalppSource->setPitch(pitch);
    }
    catch(openalpp::ValueError error) { throw ValueError(error.what()); }
    catch(openalpp::FatalError error) { throw FatalError(error.what()); }
}

float Source::getPitch() const {
    return _openalppSource->getPitch();
}

void Source::setReverbScale(float scale) throw (InitError,ValueError) {
    try {
    _openalppSource->setReverbScale(scale);
    }
    catch(openalpp::ValueError error) { throw ValueError(error.what()); }
    catch(openalpp::InitError error) { throw InitError(error.what()); }
}

void Source::setReverbDelay(float delay) throw (InitError,ValueError) {
    try {
    _openalppSource->setReverbDelay(delay);
    }
    catch(openalpp::ValueError error) { throw ValueError(error.what()); }
    catch(openalpp::InitError error) { throw InitError(error.what()); }
}

float Source::getReverbScale() throw (InitError) {
    try {
    return _openalppSource->getReverbScale();
    }
    catch(openalpp::InitError error) { throw InitError(error.what()); }
}

float Source::getReverbDelay() throw (InitError) {
    try {
    return _openalppSource->getReverbDelay();
    }
    catch(openalpp::InitError error) { throw InitError(error.what()); }
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
    try {
    _openalppSource->setPosition(x, y, z);
    }
    catch(openalpp::ValueError error) { throw ValueError(error.what()); }
    catch(openalpp::FatalError error) { throw FatalError(error.what()); }
}

void Source::getPosition(float &x, float &y, float &z) const {
    _openalppSource->getPosition(x, y, z);
}

void Source::setVelocity(float vx, float vy, float vz) {
    try {
    _openalppSource->setVelocity(vx, vy, vz);
    }
    catch(openalpp::ValueError error) { throw ValueError(error.what()); }
}

void Source::getVelocity(float &vx, float &vy, float &vz) const {
_openalppSource->getVelocity(vx, vy, vz);
}

openalpp::Source *Source::getInternalSource(void) {
 return(_openalppSource.get());
}

const openalpp::Source *Source::getInternalSource(void) const {
 return(_openalppSource.get());
}
