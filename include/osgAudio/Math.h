//$Id$

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


#ifndef OSGAUDIO_MATH_H
#define OSGAUDIO_MATH_H 1

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

namespace osgAudio {

/// Linearly interpolate from a to b using s = {0,..1}
template<typename T>
inline T const mix(T const& a, T const& b, float s)
{
  if (s > 1) 
    return b;
  if (s <=0)
    return a;

  return ((1-s)*a  + s*b);
}


/// Return the smallest item of a and b
template<typename T>
inline T const& min(T const& a, T const& b)
{
  return (a < b ? a : b);
}

/// Return the greatest item of a and b
template<typename T>
inline T const& max(T const& a, T const& b)
{
  return (a > b ? a : b);
}

/// Clamp x to fall between a and b
template<typename T>
inline T const& clamp(T const& x, T const& min, T const& max)
{
  return   ( ((x) > (max)) ? (max) : ( ((x) < (min)) ? (min) : (x) ));
}




} // namespace osgAudio

#endif // OSGAUDIO_MATH_H
