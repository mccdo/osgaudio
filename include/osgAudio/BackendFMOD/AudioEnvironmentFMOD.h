/* -*-c++-*- */
/**
 * osgAudio - OpenSceneGraph Audio Library
 * Copyright (C) 2010 AlphaPixel, LLC
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
 */

#ifndef OSGAUDIO_AUDIOENVIRONMENT_FMOD_H
#define OSGAUDIO_AUDIOENVIRONMENT_FMOD_H 1

#include <osg/ref_ptr>
#include <osg/Referenced>

#include <osgAudio/Export.h>
#include <osgAudio/Error.h>

#include <fmod.hpp>


namespace osgAudio 
{
	/**
	* Used when setting the distance model used in attenuation calculations.
	*/
	enum DistanceModel {None,InverseDistance,InverseDistanceClamped};

	/// An AudioEnvironment represents global environmental constants like Gain, Falloff and Sound Velocity.
	/*!
	This class offers a basic abstract API that adapts to various audio backends.
	It is based on the Adapter Design pattern.
	*/

	class OSGAUDIO_EXPORT AudioEnvironment : public osg::Referenced {
	public:

		/**
		* Constructor.
		*/
		AudioEnvironment( bool displayInitMsgs=false ) throw (InitError);

		/**
		* Constructor.
		* The parameters may be ignored if this isn't the first AudioEnvironment
		* object to be instantiated.
		* @param frequency is the playing frequency of the environment (in Hz)
		* @param refresh is the refresh rate of the environment (in Hz)
		* @param synchronous is true if the environment is synchronous
		*/
		AudioEnvironment(int frequency,int refresh,bool synchronous, bool displayInitMsgs=false )
			throw (InitError);


		/**
		* Constructor.
		* The parameters may be ignored if this isn't the first AudioEnvironment
		* object to be instantiated.
		* @param frequency is the playing frequency of the environment (in Hz)
		* @param refresh is the refresh rate of the environment (in Hz)
		*/
		AudioEnvironment(int frequency,int refresh=-1, bool displayInitMsgs=false )
			throw (InitError);

		/**
		* Destructor.
		*/
		~AudioEnvironment();


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
		* Initiates reverb implementation.
		* Doesn't need to be done in FMOD
		*/
		void initiateReverb() throw (InitError);

		/**
		* This is used to get the FMOD global System object, used for a
		* variety of calls.
		*/
		static FMOD::System *getSystem( bool displayInitMsgs=false ) throw (InitError);

		/**
		* Unlike openAL, FMOD requires explicit periodic "update" calls to keep
		* 3D spatialization working.
		* Static, so it can be called without having a handle to the AudioEnvironment
		*/
		static void update(void);

	private:
		void initInternals(void);

		static FMOD::System *_system;

		float _speedOfSound, _dopplerFactor, _rolloffScale, _unitScale, _gain;
		FMOD_MODE _internalDistanceModel;
		DistanceModel _externalDistanceModel;

		float calcFMODDopplerFactor(float speed) const;

	}; // AudioEnvironment

} // namespace osgAudio

#endif /* OSGAUDIO_AUDIOENVIRONMENT_FMOD_H */
