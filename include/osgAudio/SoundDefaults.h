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


#ifndef OSGAUDIO_SOUNDDEFAULTS_H
#define OSGAUDIO_SOUNDDEFAULTS_H 1

#include <osgAudio/Export.h>

#include <limits>
#include <osg/ref_ptr>
#include <osg/Object>
#include <osg/Vec3>

namespace osgAudio {

    float _init_gain(void);
    float _init_innerAngle(void);
    float _init_outerAngle(void);
    float _init_outerGain(void);
    float _init_referenceDistance(void);
    float _init_maxDistance(void);
    float _init_rolloffFactor(void);
    float _init_pitch(void);
    float _init_occludeDampingFactor(void);
    float _init_occludeScale(void);
    float _init_reverbScale(void);
    float _init_reverbDelay(void);

} // Namespace osgAudio
#endif // OSGAUDIO_SOUNDDEFAULTS_H


