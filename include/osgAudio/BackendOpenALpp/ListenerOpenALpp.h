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

#ifndef OSGAUDIO_LISTENER_OPENALPP_H
#define OSGAUDIO_LISTENER_OPENALPP_H 1

#include <osg/ref_ptr>
#include <osg/Referenced>

#include <osgAudio/Export.h>
#include <osgAudio/Error.h>

#include <openalpp/Listener.h>

namespace osgAudio 
{
    /// A Listener represents the location of an object that can collect audio from the 3D scene.
    /*!
    This class offers a basic abstract API that adapts to various audio backends.
    It is based on the Adapter Design pattern.
    */
    class OSGAUDIO_EXPORT Listener : public osg::Referenced {

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
        virtual void setVelocity(float vx, float vy, float vz);

        /**
        * Inherited from PositionedObject.
        */
        void getVelocity(float &vx, float &vy, float &vz) const;

    protected:
        /**
        * Destructor.
        */
        virtual ~Listener();

    private:
        osg::ref_ptr<openalpp::Listener> _openalppListener;

        openalpp::Listener *getInternalListener(void) {return(_openalppListener.get());};
        const openalpp::Listener *getInternalListener(void) const {return(_openalppListener.get());};

        }; // Listener

} // namespace osgAudio


#endif /* OSGAUDIO_LISTENER_OPENALPP_H */
