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

#ifndef OPENALPP_LISTENER_H
#define OPENALPP_LISTENER_H 1


#include <openalpp/Export.h>
#include <openalpp/PositionedObject.h>

namespace openalpp {

    /**
    * Class for listeners.
    */
    class OPENALPP_API Listener : public PositionedObject {
        float position_[3];
        float orientation_[6];
        float velocity_[3];
        static Listener *selectedlistener_;
        void init(float x,float y,float z,
            float vx,float vy,float vz,
            float directionx, float directiony, float directionz,
            float upx, float upy, float upz);

    protected:
        /**
        * Destructor.
        */
        virtual ~Listener();

    public:
        /**
        * Constructor.
        * Creates the listener at the default position.
        */
        Listener();


        /**
        * Copy constructor.
        */
        Listener(const Listener &listener);

        /**
        * Constructor.
        * Creates the listener at the specified position and orientation.
        * @param x x coordinate
        * @param y y coordinate
        * @param z z coordinate
        * @param directionx x value of the direction vector
        * @param directiony y value of the direction vector
        * @param directionz z value of the direction vector
        * @param upx x value of the up vector
        * @param upy y value of the up vector
        * @param upz z value of the up vector
        */
        Listener(float x,float y,float z,
            float directionx,float directiony,float directionz,
            float upx,float upy,float upz);

        /**
        * Constructor.
        * Creates the listener at the specified position.
        * @param x x coordinate
        * @param y y coordinate
        * @param z z coordinate
        */
        Listener(float x, float y, float z);

        /**
        * Select this listener.
        */
        void select();

        /**
        * Check if this listener is currently selected.
        * @return true if this listener is selected, false otherwise.
        */
        bool isSelected();

        /**
        * Set the current orientation of this listener.
        * @param directionx x value of the direction vector
        * @param directiony y value of the direction vector
        * @param directionz z value of the direction vector
        * @param upx x value of the up vector
        * @param upy y value of the up vector
        * @param upz z value of the up vector
        */
        void setOrientation(float directionx,float directiony,float directionz,
            float upx, float upy, float upz);

        /**
        * Get the current orientation of this listener.
        * @param directionx x value of the direction vector
        * @param directiony y value of the direction vector
        * @param directionz z value of the direction vector
        * @param upx x value of the up vector
        * @param upy y value of the up vector
        * @param upz z value of the up vector
        */
        void getOrientation(float &directionx,float &directiony,float &directionz, 
            float &upx, float &upy, float &upz) const;

        /**
        * Assignment operator.
        * @param listener is the object to make a copy of.
        */
        Listener &operator=(const Listener &listener);

        /**
        * Inherited from PositionedObject.
        */
        void setPosition(float x, float y, float z);

        /**
        * Inherited from PositionedObject.
        */
        void getPosition(float &x, float &y, float &z) const;

        /**
        * Inherited from PositionedObject.
        */
        void setVelocity(float vx, float vy, float vz);

        /**
        * Inherited from PositionedObject.
        */
        void getVelocity(float &vx, float &vy, float &vz) const;
    };

}

#endif /* OPENALPP_LISTENER_H */
