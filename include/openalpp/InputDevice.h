/* -*-c++-*- */
/**
 * osgAudio - OpenSceneGraph Audio Library
 * (C) Copyright 2009-2011 by Kenneth Mark Bryden
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



#ifndef OPENALPP_INPUTDEVICE_H
#define OPENALPP_INPUTDEVICE_H 1

#include <openalpp/Export.h>
#include <openalpp/Stream.h>

namespace openalpp {

/**
 * Class for handling input devices, like microphones.
 */
class OPENALPP_API InputDevice : public Stream {
  static int nobjects_;

  /**
   * Initialize input. Called by constructor(s).
   */
  void init();
 public:
  /**
   * Constructor.
   */
  InputDevice();

  /**
   * Constructor.
   * @param device is the device to open. -1 for default input.
   * @param samplerate is the desired sample rate.
   * @param buffersize is the desired buffer size _in samples_.
   * @param format is the desired sample format.
   */
  InputDevice(int device,unsigned int samplerate,unsigned int buffersize=1024, 
	      SampleFormat format=Mono16);

  /**
   * Copy constructor.
   */
  InputDevice(const InputDevice &input);

  /**
   * Assignment operator.
   */
  InputDevice &operator=(const InputDevice &input);

protected:
  /**
   * Destructor.
   */
  virtual ~InputDevice();

	ALCdevice		*pCaptureDevice_;
	unsigned long bufferSize_;
	unsigned long sampleRate_;
	SampleFormat sampleFormat_;

};

}

#endif /* OPENALPP_INPUTDEVICE_H */
