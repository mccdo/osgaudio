/* -*-c++-*- */
/**
 * osgAudio - OpenSceneGraph Audio Library
 * Copyright (C) 2010 AlphaPixel, LLC
 * based on a fork of:
 * Osg AL - OpenSceneGraph Audio Library
 * Copyright (C) 2004 VRlab, Ume� University
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
 */

#include <openalpp/DeviceUpdater.h>
#include <openalpp/Capture.h>
#include <openalpp/Sample.h>


//#include <portaudio.h>

using namespace openalpp;

void Capture::init() {

  updater_=new DeviceUpdater(-1,sampleRate_,bufferSize_,sampleFormat_,getAlBuffer(),buffer2_->getAlBuffer());
}

Capture::Capture() : bufferSize_(10000), sampleRate_(22050), sampleFormat_(Mono16)
{
  init();
}

Capture::Capture(int device,unsigned int samplerate,unsigned int buffersize,
                         SampleFormat format) : bufferSize_(buffersize), sampleRate_(samplerate),
                         sampleFormat_(format)
{
  init();

//  updater_=new DeviceUpdater(device,samplerate,buffersize*SampleFormat(format),format,buffername_,buffer2_->GetName());
}

Capture::Capture(const Capture &input)
  : Stream((const Stream &)input) {
}

Capture &Capture::operator=(const Capture &input) {
  if(this!=&input) {
    Stream::operator=((const Stream &)input);
  }
  return *this;
}

Capture::~Capture() {
  if (updater_.valid())
    updater_->stop();

  updater_=0L;
}

