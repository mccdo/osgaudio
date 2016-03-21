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

#ifndef OPENALPP_AUDIOENVIROMENT_H
#define OPENALPP_AUDIOENVIROMENT_H 1

#include <openalpp/AudioBase.h>

namespace openalpp {

    /**
    * Enum for setting/getting the current distance model.
    * None means no model is in use, i.e. no attenuation.\n
    * InverseDistance:\n
    * - \f$G=GAIN-20*log_{10}*(1+ROLLOFF*\frac{dist-REFERENCE\_DISTANCE}{REFERENCE\_DISTANCE})\f$
    * - \f$G=min (G,MAX\_GAIN)\f$
    * - \f$G=max (G,MIN\_GAIN)\f$
    *
    * InverseDistanceClamped:\n
    * - \f$dist=max (dist,REFERENCE\_DISTANCE)\f$
    * - \f$dist=min (dist,MAX\_DISTANCE)\f$
    * - And then the calculations in InverseDistance... This is equivalent to the
    * <a href="http://www.iasig.org>IASIG</a> I3DL2 distance model.\n
    *
    * In the above calculations, the variables have the following meanings:\n
    * - dist is the distance from the listener to the source.\n
    * - REFERENCE_DISTANCE are the distance at which the listener will experience
    * GAIN. Both are set per source.\n
    * - ROLLOFF is a source specific factor of attenuation. If it's set to one,
    *   the InverseDistance model will describe a "physically correct" inverse
    *   square behaviour.\n
    * - MIN_GAIN, MAX_GAIN and MAX_DISTANCE are values used for clamping gain
    *   and distance, respectively.
    */

#include <openalpp/Export.h>

#ifdef None
#undef None // Defined in X-headers
#endif
    enum DistanceModel {None,InverseDistance,InverseDistanceClamped,Linear};

    /**
    * Class for setting global parameters.
    * This doesn't have to be instantiated if one does not need to set global
    * parameters.
    */
    class OPENALPP_API AudioEnvironment : public AudioBase {
    public:
        /**
        * Constructor.
        * @param displayInitMsgs When true, writes init messages to console.
        */
        AudioEnvironment( bool displayInitMsgs=false ) throw (InitError);

        /**
        * Constructor.
        * The parameters are ignored if this isn't the first object to be
        * instantiated of the AudioBase descendants.
        * @param frequency is the playing frequency of the environment (in Hz)
        * @param refresh is the refresh rate of the environment (in Hz)
        * @param synchronous is true if the environment is synchronous
        * @param displayInitMsgs When true, writes init messages to console.
        */
        AudioEnvironment(int frequency,int refresh,bool synchronous,bool displayInitMsgs)
            throw (InitError);


        /**
        * Constructor.
        * The parameters are ignored if this isn't the first object to be
        * instantiated of the AudioBase descendants.
        * @param frequency is the playing frequency of the environment (in Hz)
        * @param refresh is the refresh rate of the environment (in Hz)
        */
        AudioEnvironment(int frequency,int refresh=-1)
            throw (InitError);

        /**
        * Sets the speed of sound in the environment.
        * This is used in Doppler calculations.
        * @param speed is the speed of sound in length units per second.
        */
        void setSoundVelocity(float speed) throw (ValueError,FatalError);

        /**
        * Get the speed of sound in the environment.
        * @return speed of sound in length units per second.
        */
        float getSoundVelocity() throw (FatalError);

        /**
        * Sets the Doppler factor.
        * This can be used to exaggerate, deemphasize or completely turn off the
        * Doppler effect.
        * @param factor has a default value of one.
        */
        void setDopplerFactor(float factor) throw (ValueError,FatalError);

        /**
        * Gets the Doppler factor.
        * @return Doppler factor.
        */
        float getDopplerFactor() throw (FatalError);

        /**
        * Sets global gain (volume).
        * The volume a source will be played at will be multiplied by this _after_
        * the attenuation calculations.
        * Note: In today's implementation on Linux, gain is clamped to [0.0,1.0].
        * This will be changed in future releases of OpenAL.
        * @param gain is the gain [0.0,...
        */
        void setGain(float gain);

        /**
        * Gets the global gain
        * @return global gain
        */
        float getGain() throw (FatalError);

        /**
        * Sets the distance model used in attenuation calculations.
        * @param model is one of: None, InverseDistance, InverseDistanceClamped.
        */
        void setDistanceModel(DistanceModel model) throw (FatalError);

        /**
        * Gets the distance model used in attenuation calculations.
        * @return the model.
        */
        DistanceModel getDistanceModel() throw (FatalError);

        /**
        * Initiates Loki's reverb implementation.
        */
        void initiateReverb() throw (InitError);
    };

}

#endif /* OPENALPP_AUDIOENVIROMENT_H */
