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


#ifndef OSGAUDIO_SOUNDSTATE_H
#define OSGAUDIO_SOUNDSTATE_H 1

#include <osgAudio/Export.h>

#include <limits>
#include <string>
#include <map>
#include <cassert>
#include <vector>
#include <list>

#include <osg/ref_ptr>
#include <osg/Object>
#include <osg/Vec3>

#include <osgAudio/Source.h>
#include <osgAudio/Listener.h>
#include <osgAudio/Sample.h>
#include <osgAudio/Stream.h>
#include <osgAudio/FileStream.h>


namespace osgAudio {
    class SoundManager;

    /// Class that encapsulates the settings valid for a soundsource 
    /*!
    This class stores the attributes for a sound source. It can exist with a sound source
    allocated to it, (hasSource() == true) which makes it the same thing as a sound source.
    Otherwise, it works as a placeholder (state) for a sound source. It can
    be assigned a sound source at any time. Then apply() is called and if it has a sound source
    the actual settings will be performed.
    If a state has a sound source all the set*() method calls apply automatically.
    */
    class OSGAUDIO_EXPORT SoundState : public osg::Object
    {
    private:
        /// Specifies what field has been set.
        enum SetField { Gain, Looping, Ambient, Relative, SoundCone, ReferenceDistance, RolloffFactor, 
            Stream, Sample, Pitch, Position, Direction, Velocity, MaxDistance, Play, Occluded, Last };

    public:

        /*!
        Constructor
        \brief Should only be used by clone methods of osg
        */
        SoundState();

        /*! 
        Constructor
        \param name - The identifier of the SoundState
        */
        SoundState( const std::string& name );

        /*! 
        Constructor
        \param name - The identifier of the SoundState
        \param sound_manager - Pointer to the SoundManager singleton
        */
        SoundState( const std::string& name, SoundManager *sound_manager );

protected:
        /// Destructor, releases any allocated sound sources
        virtual ~SoundState(); 

public:
        /*!
        Specify if the SoundState should be enabled or not.
        If it is disabled sound will not play no matter what.
        It cannot be pushed as a soundEvent to the SoundMgr
        If the soundstate has a soundsource allocated it will stop playing.
        It not, the setPlay(false) method will be called
        All parameters can be changed, but it can't be played.
        This will ensure that no-one can make a SoundState which is disabled play, unless they enable it.
        */
        void setEnable(bool flag);


        /*!
        Return if the soundstate is enabled or not.
        */
        bool getEnable() const { return m_enabled; }


        /// Copy constructor
        SoundState(const SoundState& state, const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY);

        // Implementation of virtual functions of osg::Object
        // virtual function of osg::Object
        virtual osg::Object* cloneType() const { return new SoundState(); }
        // virtual function of osg::Object
        virtual osg::Object* clone(const osg::CopyOp& copyop) const {
            return new SoundState(*this, copyop); 
        }
        /// returns "osgAudio"
        virtual const char* libraryName() const { return "osgAudio"; }
        /// returns "SoundState"
        virtual const char* className() const { return "SoundState"; }

        /// Assignment operator
        SoundState& operator=(const SoundState& state);


        /// Returns true if the SoundState has allocated a soundsource
        bool hasSource() const { return m_source != 0; }

        /// Set the sample that this state will play
        void setSample(osgAudio::Sample *sample) { m_stream = 0; m_sample = sample; set(Sample); if (m_source.valid()) apply(); }

        /// Set the stream that this state will play
        void setStream(osgAudio::Stream *stream) { m_sample = 0; m_stream = stream; set(Stream); if (m_source.valid()) apply(); }

        /// Returns the sample, if used
        const osgAudio::Sample * getSample() const {
            if(hasSource())
                if(m_sample.valid() != 0)
                    return m_sample.get();
            return NULL;
        }

        /// Returns the stream, if used
        const osgAudio::Stream * getStream() const {
            if(hasSource())
                if(m_stream.valid() != 0)
                    return m_stream.get();
            return NULL;
        }

        /// Set the position of SoundState
        void setPosition(const osg::Vec3& pos) { m_position = pos; set(Position); if (m_source.valid()) apply(); }

        /// Get the position of SoundState
        osg::Vec3 getPosition() const { return m_position; }

        /// Set the velocity of the SoundState 
        void setVelocity(const osg::Vec3& vel) { m_velocity = vel; set(Velocity); if (m_source.valid()) apply(); }

        /// Get the velocity of the SoundState 
        osg::Vec3 getVelocity() const { return m_velocity; }


        /// Set the direction of the SoundState
        void setDirection(const osg::Vec3& dir) { m_direction = dir; set(Direction); if (m_source.valid()) apply(); }

        /// Get the direction of the SoundState
        osg::Vec3 getDirection() const { return m_direction; }

        /// Set the gain (volume) of the soundstate (1.0 is default)
        void setGain(float gain) { m_gain = gain; set(Gain); if (m_source.valid()) apply(); }

        /// Get the gain (volume) of the soundstate (1.0 is default)
        float getGain() const { return m_gain; }

        /// Return true if the sound state is in looping mode
        bool getLooping() const { return m_looping; }

        /// Set the SoundState in looping mode
        void setLooping(bool flag) {  m_looping = flag; set(Looping); if (m_source.valid()) apply(); }

        /// Set the soundstate to ambient (no attenuation will be calculated)
        void setAmbient(bool flag) {  m_ambient = flag; set(Ambient); if (m_source.valid()) apply(); }

        /// Get if the soundstate is ambient (no attenuation will be calculated)
        bool getAmbient() const {  return m_ambient; }

        /// Set the soundstate so its position will always be relative to the listener
        void setRelative(bool flag) { m_relative = flag; set(Relative); if (m_source.valid()) apply(); }

        /// Get if the soundstate's position will always be relative to the listener
        bool getRelative() const { return m_relative; }


        /*! 
        Specifies the sound cone for a directional sound source.
        When outside the cone, the outer gain is the gain used in the attenuation calculation.
        */
        void setSoundCone(float innerAngle, float outerAngle, float outerGain) 
        { m_innerAngle = innerAngle; m_outerAngle = outerAngle; m_outerGain = outerGain; set(SoundCone); 
        if (m_source.valid()) apply(); 
        }

        /// Get the the inner angle of the cone for the SoundState in degrees
        float getInnerAngle() const { return m_innerAngle; }

        /// Get the the outer angle of the cone for the SoundState in degrees
        float getOuterAngle() const { return m_outerAngle; }

        /// Get the the outer gain of the cone for the SoundState
        float getOuterGain() const { return m_outerGain; }

        /// \return true if the source is in playing state. If there is no source associated, it will return false.
        bool isActive();

        /// Set the reference distance for the SoundState
        void setReferenceDistance(float distance) { m_referenceDistance = distance; set(ReferenceDistance); if (m_source.valid()) apply(); }

        /// Get the reference distance for the SoundState
        float getReferenceDistance() const { return m_referenceDistance; }

        /*! Set the maximum distance for the SoundState. Further away from the listener the source will be
        turned off when in the InverseClamp sound mode 
        1.0 is default
        */
        void setMaxDistance(float max) { m_maxDistance = max; set(MaxDistance); if (m_source.valid()) apply(); }

        /*! Get the maximum distance for the SoundState. Further away from the listener the source will be
        turned of when in the InverseClamp sound mode 
        1.0 is default
        */
        float getMaxDistance() const { return m_maxDistance; }

        /// Specifies the roll-off factor for the SoundState, 1.0 is default
        void setRolloffFactor(float roll) {m_rolloffFactor = roll; set(RolloffFactor); if (m_source.valid()) apply(); }

        /// Return the roll-off factor for the SoundState, 1.0 is default
        float getRolloffFactor() const {return m_rolloffFactor; }

        /// Set the pitch (rate) for the SoundState (1.0 is default)
        void setPitch(float pitch) {  m_pitch = pitch; set(Pitch); if (m_source.valid()) apply(); }

        /// Get the pitch (rate) for the SoundState (1.0 is default)
        float getPitch() const { return m_pitch; }

        /// Starts to play the SoundState
        void setPlay(bool flag) { m_play = flag; set(Play); if (m_source.valid()) apply(); }

        /// Return if the soundstate is playing
        bool getPlay() { return m_play; }

        void setOccludeDampingFactor(float d) { m_occlude_damping_factor = d; }
        float getOccludeDampingFactor() const { return m_occlude_damping_factor; }

        void setOccludeScale(float d) { m_occlude_scale = d; }
        float getOccludeScale() const { return m_occlude_scale; }

        void setOccluded(bool f) { m_is_occluded = f; set(Occluded); if (m_source.valid()) apply(); }
        bool getOccluded() const { return m_is_occluded; }

        /// Set whether pause or stop should be used when calling setPlay(false) 
        void setStopMethod(osgAudio::SourceState s) 
        { 
            if ( s == osgAudio::Paused ) 
                m_pause = true;
            else
                m_pause = false;
        }

        /// Returns true if there is a source and it is playing
        bool isPlaying() { return (m_source.valid() && m_source->getState() == osgAudio::Playing); }

        /// const Returns true if there is a source and it is playing
        bool isPlaying() const { return (m_source.valid() && m_source->getState() == osgAudio::Playing); }

        // Get stop method -  used by when calling setPlay(false)
        osgAudio::SourceState getStopMethod()  const
        { 
            if (m_pause)
                return osgAudio::Paused;
            else
                return osgAudio::Stopped;
        }

        /*! Allocates a soundsource for this soundstate
        \param priority - This priority is used when fighting with other active soundstates for the limited resource of SoundSources.
        If an active (playing) SoundState has a lower priority than this, it will be discarded, and the sound source will be used for this
        SoundState. The higher, the better chance of actually getting a Sound source allocated.
        \param register_as_active - This specifies whether the Allocated SoundSource will be added to the list of active SoundSources.
        \return true if a source was successfully allocated.
        */
        bool allocateSource(unsigned int priority=0, bool register_as_active=true); 

        /// Return the priority set for this SoundState
        unsigned int getPriority() const { return m_priority; }


        /// Set the Source for this SoundState
        void setSource(Source *source); 

        /// Return the Source for this SoundState
        Source *getSource() { return m_source.get(); }

        /// Return the const Source for this SoundState
        const Source *getSource() const { return m_source.get(); }

        /// Release the Source that is allocated for this SoundState
        void releaseSource();


        /*! Performs the actual modification to the allocated Source.
        Checks the bits in m_is_set to see what attributes have changed
        since last apply and does a lazy update. */
        void apply(); 

    private:
        SoundManager *m_sound_manager;

        /// Clear all the flags indicating a value has been set
        void setAll(bool flag);

        osg::ref_ptr<osgAudio::Stream> m_stream;
        osg::ref_ptr<osgAudio::Sample> m_sample;
        osg::ref_ptr<osgAudio::Source> m_source;

        osg::Vec3 m_position;
        osg::Vec3 m_direction;
        osg::Vec3 m_velocity;
        float m_gain, m_innerAngle, m_outerAngle, m_outerGain; 
        float m_referenceDistance, m_maxDistance;
        float m_rolloffFactor, m_pitch;
        float m_occlude_damping_factor, m_occlude_scale;
        bool m_is_occluded;

        bool m_looping, m_ambient, m_relative, m_play, m_pause;
        unsigned m_priority;
        bool m_enabled;



        /// Return true if SetField f is set since last call to apply() or clear()
        bool isSet(SetField f) const { return ((m_is_set>>f)&01) != 0; } 
        bool isNoneSet() const { return m_is_set==0; }

        /// Set the given field to true
        void set(SetField f) { m_is_set |=  (01 << f); } 

        /// Clear the given field to false
        void clear(SetField f)  { m_is_set &=  ~(01 << f); }

        unsigned long m_is_set;

    };

    // For convenience, use by external apps
    typedef std::list< osg::ref_ptr< SoundState > > SoundStateList;
    typedef std::vector< osg::ref_ptr< SoundState > > SoundStateVector;

} // Namespace osgAudio
#endif // OSGAUDIO_SOUNDSTATE_H


