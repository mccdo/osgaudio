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

#ifndef OPENALPP_POSITIONEDOBJECT_H
#define OPENALPP_POSITIONEDOBJECT_H 1

#include <openalpp/Export.h>
#include <openalpp/AudioBase.h>

namespace openalpp {

    /**
    * Virtual base class for positioned objects.
    * (I.e. listeners and sources).
    */
    class OPENALPP_API PositionedObject : public AudioBase {

    public:

        PositionedObject() {}

        /**
        * Set position.
        * @param x x coordinate.
        * @param y y coordinate.
        * @param z z coordinate.
        */
        virtual void setPosition(float x, float y, float z)=0;

        /**
        * Get position.
        * @param x x coordinate.
        * @param y y coordinate.
        * @param z z coordinate.
        */
        virtual void getPosition(float &x, float &y, float &z) const =0;

        /**
        * Set velocity.
        * @param vx x member of velocity vector.
        * @param vy y member of velocity vector.
        * @param vz z member of velocity vector.
        */
        virtual void setVelocity(float vx, float vy, float vz)=0;

        /**
        * Get velocity.
        * @param vx x member of velocity vector.
        * @param vy y member of velocity vector.
        * @param vz z member of velocity vector.
        */
        virtual void getVelocity(float &vx, float &vy, float &vz) const =0;

    protected:
        virtual ~PositionedObject() {}

    };

}

#endif /* OPENALPP_POSITIONEDOBJECT_H */
