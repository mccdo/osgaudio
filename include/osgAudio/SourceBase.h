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

#ifndef OSGAUDIO_SOURCEBASE_H
#define OSGAUDIO_SOURCEBASE_H 1
#include <osg/ref_ptr>
#include <osg/Referenced>

#include <osgAudio/Export.h>
#include <osgAudio/Error.h>

namespace osgAudio {

    /**
    * Enum for the different states of a source.
    * When a source is created, it is in the Initial state. Rewind will return a
    * source to this state.
    * The other states should be obvious.
    */
    enum SourceState {Initial,Playing,Paused,Stopped};

    /**
    * Base class for sources.
    * This class holds functions for playing, setting position etc. However, it
    * cannot be instantiated, instead a source of either type (GroupSource or
    * Source) should be created.
    */
    class OSGAUDIO_EXPORT SourceBase : public osg::Referenced {
    public:
        /**
        * Play the source.
        */
        virtual void play() = 0;

        /**
        * Pause the source.
        */
        virtual void pause() = 0;

        /**
        * Stop the source.
        */
        virtual void stop() = 0;

        /**
        * Rewind the source.
        */
        virtual void rewind() = 0;

        /**
        * Get the current state.
        * @return one of Initial,Playing,Paused,Stopped
        */
        virtual SourceState getState() const = 0;

        /**
        * Turn on/off looping.
        * @param loop is true if the source should loop, false otherwise.
        */
        virtual void setLooping(bool loop = true) = 0;

        /**
        * Check whether the source is looping.
        * @return true if it's looping, false otherwise.
        */
        virtual bool isLooping() const = 0;

        /**
        * @return true if source is paused, false otherwise.
        */
        virtual bool isPaused() const = 0;

        /**
        * Sets the direction of the source.
        * @param directionx x value of the direction vector.
        * @param directiony y value of the direction vector.
        * @param directionz z value of the direction vector.
        */
        virtual void setDirection(float directionx, float directiony, float directionz) = 0;

        /**
        * Gets the direction of the source.
        * @param directionx x value of the direction vector.
        * @param directiony y value of the direction vector.
        * @param directionz z value of the direction vector.
        */
        virtual void getDirection(float &directionx,float &directiony,float &directionz) const = 0;

        /**
        * Makes the source omni-directional.
        * The same effect can be achieved by calling SetDirection(0,0,0)
        */
        virtual void makeOmniDirectional() = 0;

        /**
        * Sets the sound cone parameters for a directional sound source.
        * This function has no effect on omni-directional sources. Two cones, with
        * the top at the source, and turned the same direction as the source,
        * are defined by this. Inside the inner cone (specified by innerangle),
        * sound will be played at full volume (attenuated by distance), and
        * outside the outer cone (specified by outerangle) sound will be played
        * at the volume specified by outergain. Between these areas, the sound
        * volume will be interpolated between normal gain and outergain.
        * @param innerangle specifies the inner cone.
        * @param outerangle specifies the outer cone.
        * @param outergain specifies the gain outside the outer cone.
        */
        virtual void setSoundCone(float innerangle,
            float outerangle=360.0,float outergain=0.0) = 0;

        /**
        * Gets the sound cone parameters.
        * @param innerangle specifies the inner cone.
        * @param outerangle specifies the outer cone.
        * @param outergain specifies the gain outside the outer cone.
        */
        virtual void getSoundCone(float &innerangle,float &outerangle,float &outergain) const = 0;

        /**
        * Sets gain (volume).
        * The volume a source will be played at will be multiplied by this _after_
        * the attenuation calculations.
        * Note: In today's implementation on Linux, gain is clamped to [0.0,1.0].
        * This will be changed in future releases of OpenAL.
        * @param gain is the gain [0.0,...
        */
        virtual void setGain(float gain) = 0;

        /**
        * Gets the gain (volume).
        * @return gain.
        */
        virtual float getGain() const = 0;

        /**
        * Sets maximum and minimum gain this source will be played at.
        * I.e. the gain will be clamped to these values.
        * @param min is minimum gain.
        * @param max is maximum gain.
        */
        virtual void setMinMaxGain(float min = 0.0, float max = 1.0) = 0;

        /**
        * Gets maximum and minimum gain.
        * @param min is minimum gain.
        * @param max is maximum gain.
        */
        virtual void getMinMaxGain(float & min, float &max) const = 0;

        /**
        * Makes the source ambient (or makes it stop being ambient).
        * This function _will_ change the source's position, direction and roll-off
        * factor.
        * @param ambient is true if the source should be ambient, false otherwise.
        */
        virtual void setAmbient(bool ambient = true) = 0;

        /**
        * Check if the source is ambient.
        * @return true if the source is ambient, false otherwise.
        */
        virtual bool isAmbient() const = 0;

        /**
        * Makes the source's position relative (or makes it stop being relative).
        * This means that the position in space will not be absolute, but rather
        * relative to the listener.
        * @param relative is true if the source should be relative, false otherwise.
        */
        virtual void setRelative(bool relative = true) = 0;

        /**
        * Check if the source's position is relative.
        * @return true if the source is relative, false otherwise.
        */
        virtual bool isRelative() const = 0;

        /**
        * Sets the reference distance for this source.
        * The reference distance is used in attenuation calculations.
        * @param distance is the reference distance.
        */
        virtual void setReferenceDistance(float distance = 1.0) = 0;

        /**
        * Gets the reference distance.
        * @return reference distance.
        */  
        virtual float getReferenceDistance() const = 0;

        /**
        * Sets the maximum distance.
        * This is used in attenuation calculations, if the distance model is
        * InverseDistanceClamped.
        * @param distance is the maximum distance.
        */
        virtual void setMaxDistance(float distance) = 0; // = MAX_FLOAT);

        /**
        * Gets the maximum distance.
        * @return maximum distance.
        */
        virtual float getMaxDistance() const = 0;

        /**
        * Sets the roll-off factor.
        * This is used in distance attenuation calculations.
        * @param factor is the roll-off factor.
        */
        virtual void setRolloffFactor(float factor = 1.0) = 0;

        /**
        * Gets the roll-off factor.
        * @return roll-off factor.
        */
        virtual float getRolloffFactor() const = 0;

        /**
        * Sets the pitch.
        * 1.0 is normal. Each reduction by 50% equals a reduction by one octave.
        * @param pitch is the pitch (0.0,1.0].
        */
        virtual void setPitch(float pitch = 1.0) = 0;

        /**
        * Gets the pitch.
        * @return pitch.
        */
        virtual float getPitch() const = 0;

        /** 
        * Set reverb scale for this source.
        * This is simply the scale of the "echo."
        * AudioEnviroment::InitiateReverb() must be called before this.
        * @param scale is the reverb scale [0.0-1.0].
        */
        virtual void setReverbScale(float scale) throw (InitError,ValueError) = 0;

        /**
        * Set reverb delay for this source.
        * AudioEnviroment::InitiateReverb() must be called before using this.
        * This is how many seconds back in time the echo will be.
        * @param delay is the delay [0.0-2.0] in seconds.
        */
        virtual void setReverbDelay(float delay) throw (InitError,ValueError) = 0;

        /**
        * Get reverb delay for this source.
        * @return the delay.
        */
        virtual float getReverbDelay() throw (InitError) = 0;

        /**
        * Get reverb scale for this source.
        * @return the scale.
        */
        virtual float getReverbScale() throw (InitError) = 0;

        /**
        * Link this source to another.
        * This causes calls to Play(), Pause(), Stop() and Rewind() (on this source)
        * to be applied to all sources this has been linked to, synchronously.
        * @param source is the source to link to.
        * @return identifier for the linked source. This is also the OpenAL name for
        * it.
        */
        // <<<>>> May need to re-engineer
        //virtual unsigned int link(const SourceBase *source) throw (MemoryError) = 0;

        /**
        * Unlink this source from another.
        * @param source is the source to unlink.
        */
        // <<<>>> May need to re-engineer
        //virtual void unlink(const SourceBase *source) throw (NameError) = 0;

        /**
        * Unlink this source from another.
        * @param name is the name of the source to unlink.
        */
        // <<<>>> May need to re-engineer
        //virtual void unlink(const unsigned int name) throw (NameError) = 0;

        /**
        * Unlink all sources from this.
        */
        // <<<>>> May need to re-engineer
        //virtual void unlinkAll() = 0;

        /**
        * Returns the OpenAL name of the source.
        * Can be used to directly modify the source with OpenAL functions.
        * Probably needs to be removed or modified
        */
        //virtual unsigned int getAlSource() const = 0;

        /**
        * Inherited from PositionedObject.
        */
        virtual void setPosition(float x, float y, float z) = 0;

        /**
        * Inherited from PositionedObject.
        */
        virtual void getPosition(float &x, float &y, float &z) const = 0;

        /**
        * Inherited from PositionedObject.
        */
        virtual void setVelocity(float vx, float vy, float vz) = 0;

        /**
        * Inherited from PositionedObject.
        */
        virtual void getVelocity(float &vx, float &vy, float &vz) const = 0;

        /**
        * Assignment operator.
        */
        //SourceBase &operator=(const SourceBase &sourcebase);

    protected:
        /**
        * Destructor.
        */
        virtual ~SourceBase() {};

        /**
        * Constructor.
        */
        //SourceBase() throw (MemoryError,NameError);

        /**
        * Constructor.
        * @param x x coordinate.
        * @param y y coordinate.
        * @param z z coordinate.
        */
        //SourceBase(float x,float y,float z) throw (MemoryError,NameError);

        /**
        * Copy constructor.
        */
        //SourceBase(const SourceBase &sourcebase);

    };

} // osgAudio

#endif /* OSGAUDIO_SOURCEBASE_H */
