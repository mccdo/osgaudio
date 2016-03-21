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

#include <openalpp/Source.h>
#include <openalpp/FileStream.h>

using namespace openalpp;

Source::Source(float x, float y, float z) : SourceBase(x,y,z) {
    streaming_=false;
    sounddata_=NULL;
}

Source::Source(const std::string& filename) : SourceBase() {

    setSound(new Sample(filename));
}


Source::Source( Stream *stream) : SourceBase() {

    setSound(stream);
}

Source::Source( Sample *sample) : SourceBase() {

    setSound(sample);
}

Source::Source( const Source &source) : SourceBase(source) {

    streaming_=source.streaming_;
    if(streaming_)
        sounddata_= new Stream(*(const Stream *)source.getSound());
    else
        sounddata_= new Sample(*(const Sample *)source.getSound());

    alSourcei(sourcename_,AL_BUFFER,sounddata_->getAlBuffer());
    ALCHECKERROR();
}

Source::~Source() {
    stop();
    sounddata_ = 0;
}

// TODO: Add flag for whether the sound should be loaded. This is useful for
// dummy sources that will be used only for groupsources.
void Source::setSound(const std::string& filename) {
    streaming_=false;

    // Detach any previous buffers
    if (sounddata_.valid()) {
        stop();
        alSourcei(sourcename_,AL_BUFFER,0);

    }

    sounddata_=new Sample(filename);
    alSourcei(sourcename_,AL_BUFFER,sounddata_->getAlBuffer());
    ALCHECKERROR();
}

void Source::setSound( Sample *buffer ) {


    streaming_=false;

    // Detach any previous buffers
    if (sounddata_.valid()) {
        stop();
        alSourcei(sourcename_,AL_BUFFER,0);
        ALCHECKERROR();
    }

    sounddata_=buffer;

    alSourcei(sourcename_,AL_BUFFER,sounddata_->getAlBuffer());
    ALCHECKERROR();

}

void Source::setSound(Stream *stream) {
    streaming_=true;

    if (sounddata_.valid())
    {
        std::cerr << "Source::setSound(Stream *stream) resetting stream not supported, create new source" << std::endl;
        throw FatalError("Source::setSound(Stream *stream) resetting stream not supported, create new source");
    }

    sounddata_= stream;
    alSourcei(sourcename_,AL_BUFFER,sounddata_->getAlBuffer());
    ALCHECKERROR();
}

const SoundData *Source::getSound() const {
    return sounddata_.get();
}

void Source::play(const std::string& filename) {
    setSound(filename);
    SourceBase::play();
}

void Source::play( Sample *buffer) {
    setSound(buffer);
    SourceBase::play();
}

void Source::play( Stream *stream ) {

    alSourcei(sourcename_,AL_LOOPING,AL_FALSE); //Streaming sources can't loop...
    ALCHECKERROR();
    setSound(stream);
    ((Stream *)sounddata_.get())->record(sourcename_);
    SourceBase::play();
}

void Source::setLooping(bool loop) {
    if (streaming_) {

        FileStream *stream = dynamic_cast<FileStream *>(sounddata_.get());
        if (stream)
            stream->setLooping(loop);
    }
    else 
        SourceBase::setLooping(loop);
}

void Source::play() {

    if(streaming_ && !isPaused()) {
        alSourcei(sourcename_,AL_LOOPING,AL_FALSE); //Streaming sources can't loop...
        ALCHECKERROR();
        // Remove any previous attached buffers if its a Source attached to a stream

        ((Stream *)sounddata_.get())->record(sourcename_);
    }
    SourceBase::play();
}

void Source::seek(float time_s)
{
    if (streaming_)
    {
        /* continuing to use downcast for this to avoid 
        ** modifying SoundData for now */
        static_cast<Stream *>(sounddata_.get())->seek(time_s);
    }
}

void Source::stop() {
    if(streaming_)
        ((Stream *)sounddata_.get())->stop(sourcename_);
    SourceBase::stop();
}

void Source::pause() {
    SourceBase::pause();
}


bool Source::isStreaming() {
    return streaming_;
}

Source &Source::operator=(const Source &source) {
    if(this!=&source) {
        SourceBase::operator=(source);

        streaming_=source.streaming_;
        if(streaming_)
            sounddata_=new Stream(*(Stream *)source.sounddata_.get());
        else
            sounddata_=new Sample(*(Sample *)source.sounddata_.get());
    }
    return *this;
}

