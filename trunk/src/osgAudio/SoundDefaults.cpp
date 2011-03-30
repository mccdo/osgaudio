/* -*-c++-*- */
/**
 * osgAudio - OpenSceneGraph Audio Library
 * (C) Copyright 2009-2011 by Kenneth Mark Bryden
 * (programming by Chris 'Xenon' Hanson, AlphaPixel, LLC xenon at alphapixel.com)
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

#include <osgAudio/SoundDefaults.h>

namespace osgAudio {

	float _init_gain(void) {return(1.0f);}
	float _init_innerAngle(void) {return(360.0f);}
	float _init_outerAngle(void) {return(360.0f);}
	float _init_outerGain(void) {return(0.0f);}
	float _init_referenceDistance(void) {return(1.0f);}
	float _init_maxDistance(void) {return(std::numeric_limits<float>::max());}
	float _init_rolloffFactor(void) {return(1.0f);}
	float _init_pitch(void) {return(1.0f);}
	float _init_occludeDampingFactor(void) {return(0.5f);}
	float _init_occludeScale(void) {return(1.0f);}
	float _init_reverbScale(void) {return(0.25f);}
	float _init_reverbDelay(void) {return(0.0f);}

} // Namespace osgAudio

