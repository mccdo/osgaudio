/* -*-c++-*- */
/**
 * osgAudio - OpenSceneGraph Audio Library
 * (C) Copyright 2009-2012 byKenneth Mark Bryden
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

#ifndef OSGAUDIO_AUDIOENVIRONMENT_FMOD_H
#define OSGAUDIO_AUDIOENVIRONMENT_FMOD_H 1

#include <osg/ref_ptr>

#include <osgAudio/Export.h>
#include <osgAudio/Error.h>

#include <fmod.hpp>


namespace osgAudio 
{
	/**
	* Used when setting the distance model used in attenuation calculations.
	*/
	enum DistanceModel {None,InverseDistance,InverseDistanceClamped,Linear};

	/// An AudioEnvironment represents global environmental constants like Gain, Falloff and Sound Velocity.
	/*!
	This class offers a basic abstract API that adapts to various audio backends.
	It is based on the Adapter Design pattern.
	*/

	class OSGAUDIO_EXPORT AudioEnvironment
    {
	public:
		/// Return a pointer to the singleton object
		static AudioEnvironment* instance();

		/**
		 * Initialize the FMOD environment.
		 */
		void init( bool displayInitMsgs=false ) throw (InitError);

		/**
		 * Initialize the FMOD environment.
		 * The parameters may be ignored if this isn't the first AudioEnvironment
		 * object to be instantiated.
		 * @param frequency is the playing frequency of the environment (in Hz)
		 * @param refresh is the refresh rate of the environment (in Hz)
		 * @param synchronous is true if the environment is synchronous
		 */
		void init(int frequency,int refresh,bool synchronous, bool displayInitMsgs=false )
			throw (InitError);

		/**
		 * Initialize the FMOD environment.
		 * The parameters may be ignored if this isn't the first AudioEnvironment
		 * object to be instantiated.
		 * @param frequency is the playing frequency of the environment (in Hz)
		 * @param refresh is the refresh rate of the environment (in Hz)
		 */
		void init(int frequency,int refresh=-1, bool displayInitMsgs=false )
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
		 * Sets the unit scale. Not used by OpenAL.
		 * From FMOD: "How many units per meter does your engine have". For example, if you are using feet then "scale" would equal 3.28.
		 * According to FMOD this only affects Doppler calculations, not attenuation:
		 * "This only affects doppler! If you keep your min/max distance, custom rolloff curves and positions in scale relative to each other the volume rolloff will not change. If you set this, the mindistance of a sound will automatically set itself to this value when it is created in case the user forgets to set the mindistance to match the new distancefactor."
		 * @param factor has a default value of one.
		 */
		void setUnitScale(float scale) throw (ValueError,FatalError);

		/**
		 * Gets the unit scale.  Not used by OpenAL.
		 * @return Unit scale.
		 */
		float getUnitScale() throw (FatalError);

		/**
		 * Sets global gain (volume).
		 * The volume a source will be played at will be multiplied by this _after_
		 * the attenuation calculations.
		 * Note: In some implementations gain is clamped to [0.0,1.0].
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
		 * Gets the internal FMOD distance model used in attenuation calculations.
		 * Used internally by Stream and Sample creation code.
		 * @return FMOD_3D_LOGROLLOFF or FMOD_3D_LINEARROLLOFF.
		 */
		FMOD_MODE getInternalDistanceModel() const {return(_internalDistanceModel);}

		/**
		 * Initiates reverb implementation.
		 * Doesn't need to be done in FMOD
		 */
		void initiateReverb() throw (InitError);

		/**
		 * This is used to get the FMOD global System object, used for a
		 * variety of calls.
		 */
        FMOD::System *getSystem( bool displayInitMsgs=false ) throw (InitError);

		/**
		 * Unlike openAL, FMOD requires explicit periodic "update" calls to keep
		 * 3D spatialization working.
		 * Static, so it can be called without having a handle to the AudioEnvironment
		 */
        void update();

        /**
         * Set the function pointer that will be called before the init 
         * function is called on _system.
         */
        void setPreSystemInitFunc( void (*preSystemInitFunc)(FMOD::System*) );

	private:
        /**
		 * Destructor.
		 */
		~AudioEnvironment();

        /**
		 * Constructor.
		 */
		AudioEnvironment();
        
		void initInternals();

        FMOD::System* _system;

		float _speedOfSound, _dopplerFactor, _rolloffScale, _unitScale, _gain;
		FMOD_MODE _internalDistanceModel;
		DistanceModel _externalDistanceModel;

		float calcFMODDopplerFactor(float speed) const;
        
        void (*_preSystemInitFunc)(FMOD::System*);

	}; // AudioEnvironment

} // namespace osgAudio

#endif /* OSGAUDIO_AUDIOENVIRONMENT_FMOD_H */
