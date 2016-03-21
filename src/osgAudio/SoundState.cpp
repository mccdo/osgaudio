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

// Force the use of the stl max function not the windows.h function
#ifdef WIN32
#define NOMINMAX
#endif

#include <osg/Notify>

#include <osgAudio/SoundState.h>
#include <osgAudio/SoundManager.h>
#include <osgAudio/SoundDefaults.h>

#include <limits>
const float _init_maxDistance = std::numeric_limits< float >::max();

using namespace osgAudio;

SoundState::SoundState( const std::string& name ) 
    : 
    m_sound_manager(SoundManager::instance()), 
    m_gain(_init_gain()), 
    m_innerAngle(_init_innerAngle()), 
    m_outerAngle(_init_outerAngle()), 
    m_outerGain(_init_outerGain()), 
    m_referenceDistance(_init_referenceDistance()), 
    m_maxDistance(_init_maxDistance()),
    m_rolloffFactor(_init_rolloffFactor()), 
    m_pitch(_init_pitch()), 
    m_occlude_damping_factor(_init_occludeDampingFactor()),
    m_occlude_scale(_init_occludeScale()),  
    m_is_occluded(false), 
    m_looping(false),
    m_ambient(false), 
    m_relative(false),
    m_play(false), 
    m_pause(false), 
    m_priority(0), 
    m_enabled(true),
    m_is_set(0)
{
    setName(name);
}

SoundState::SoundState( const std::string& name, SoundManager *sound_manager ) 
    : 
    m_sound_manager(sound_manager), 
    m_gain(_init_gain()), 
    m_innerAngle(_init_innerAngle()), 
    m_outerAngle(_init_outerAngle()), 
    m_outerGain(_init_outerGain()), 
    m_referenceDistance(_init_referenceDistance()), 
    m_maxDistance(_init_maxDistance()),
    m_rolloffFactor(_init_rolloffFactor()), 
    m_pitch(_init_pitch()), 
    m_occlude_damping_factor(_init_occludeDampingFactor()),
    m_occlude_scale(_init_occludeScale()),  
    m_is_occluded(false), 
    m_looping(false),
    m_ambient(false), 
    m_relative(false),
    m_play(false), 
    m_pause(false), 
    m_priority(0), 
    m_enabled(true),
    m_is_set(0)
{
    setName(name);
}


SoundState::SoundState() 
    : 
    m_sound_manager(SoundManager::instance()), 
    m_gain(_init_gain()), 
    m_innerAngle(_init_innerAngle()), 
    m_outerAngle(_init_outerAngle()), 
    m_outerGain(_init_outerGain()), 
    m_referenceDistance(_init_referenceDistance()), 
    m_maxDistance(_init_maxDistance()),
    m_rolloffFactor(_init_rolloffFactor()), 
    m_pitch(_init_pitch()), 
    m_occlude_damping_factor(_init_occludeDampingFactor()),
    m_occlude_scale(_init_occludeScale()),  
    m_is_occluded(false), 
    m_looping(false),
    m_ambient(false), 
    m_relative(false),
    m_play(false), 
    m_pause(false), 
    m_priority(0), 
    m_enabled(true),
    m_is_set(0)
{ 
}


SoundState::SoundState(const SoundState& state, const osg::CopyOp& copyop) : osg::Object(state, copyop) 
{
    m_sound_manager = state.m_sound_manager;
    *this = state;
}


SoundState& SoundState::operator=(const SoundState& state)
{
    if (this == &state)
        return *this;

    m_stream =            state.m_stream;
    m_sample =            state.m_sample;
    m_gain =              state.m_gain;
    m_looping =           state.m_looping;
    m_ambient =           state.m_ambient;
    m_relative =          state.m_relative;
    m_innerAngle =        state.m_innerAngle;
    m_outerAngle =        state.m_outerAngle;
    m_outerGain =         state.m_outerGain;
    m_referenceDistance = state.m_referenceDistance;
    m_maxDistance =       state.m_maxDistance;
    m_rolloffFactor =     state.m_rolloffFactor;
    m_pitch =             state.m_pitch;
    m_position =          state.m_position;
    m_direction =         state.m_direction;
    m_velocity =          state.m_velocity;
    m_priority =          state.m_priority;
    m_play =              state.m_play;
    m_pause =             state.m_pause;
    m_is_occluded =       state.m_is_occluded;
    m_occlude_scale =     state.m_occlude_scale;
    m_occlude_damping_factor = state.m_occlude_damping_factor;
    m_enabled =                        state.m_enabled;
    m_is_set =                        state.m_is_set;

    if (state.m_source.valid())
        if (!allocateSource(m_priority))
            throw std::runtime_error("SoundState::operator=():  No soundsources available during Assignment");

    // Indicate that all fields have been changed
    setAll(true);

    return *this;
}



bool SoundState::allocateSource(unsigned int priority, bool register_as_active) 
{ 
    m_source= m_sound_manager->allocateSource(priority, register_as_active);

    if (!m_source.valid()) 
        return false;

    m_priority = priority; 

    if (m_source.valid()) {
        setAll(true); // Apply all parameters from soundstate to the just recently allocated sound source
        apply(); 
    }

    return true; 
}

void SoundState::releaseSource()
{ 

    if (m_source.valid()) 
        m_sound_manager->releaseSource(m_source.get()); 
    m_source = 0; 
}


bool SoundState::isActive() 
{ 
    bool f; 
    if (!m_source.valid()) 
        f= false; 
    else 
        f= (m_source->getState() == osgAudio::Playing); 

    //info("isActive") << "Active: " << f << std::endl; 
    return f;
}


SoundState::~SoundState() 
{ 
    releaseSource(); 
}


void SoundState::setSource(Source *source)
{ 
    assert((m_sample.valid() || m_stream.valid()) && "setSource: No stream or sample associated to the soundstate"); 

    if (m_source.valid()) 
        releaseSource(); 
    m_source = source;
    // The following code is believed to be un-necessary. It is benign or
    // mostly harmless for openAL, but causes a resource leak of an FMOD
    // channel (FMOD refcounts playing or paused channels) because it allocates
    // a channel but we never get around to telling it to play and apply()
    // below repeats the process (and then does play the second-allocated channel).
/*
    if (m_sample.valid())
        m_source->setSound(m_sample.get()); 
    else if (m_stream.valid())
        m_source->setSound(m_stream.get()); 
*/
}

///
void SoundState::apply()
{
    if (isNoneSet())
        return;

    if (!m_source.valid())
        throw std::runtime_error("SoundState::apply(): No sound source allocated.");

    if (!m_sample.valid() && !m_stream.valid()) {
        //osg::notify(osg::WARN) << "SoundState::apply(): No sample or stream assigned to SoundState" << std::endl;
        return;
    }

    if (isSet(Stream) && m_stream.valid())
        m_source->setSound(m_stream.get());

    if (isSet(Sample) && m_sample.valid())
        m_source->setSound(m_sample.get());

    if (isSet(Position))
        m_source->setPosition(m_position[0], m_position[1], m_position[2]); 

    if (isSet(Direction))
        m_source->setDirection(m_direction[0], m_direction[1], m_direction[2]); 

    if (isSet(Gain))
        m_source->setGain(m_gain);

    if (isSet(Occluded)) {
        if (m_is_occluded)  {
            float g = m_gain*( 1 + (m_occlude_scale-1)*m_occlude_damping_factor);
            m_source->setGain(g);
        }
        else {
            m_occlude_scale = 1.0f;
            m_source->setGain(m_gain);
        }

    }

    if (isSet(Looping))
        m_source->setLooping(m_looping);


    if (isSet(SoundCone))
        m_source->setSoundCone(m_innerAngle, m_outerAngle, m_outerGain);

    if (isSet(ReferenceDistance))    
        m_source->setReferenceDistance(m_referenceDistance);

    if (isSet(MaxDistance))
        m_source->setMaxDistance(m_maxDistance);

    if (isSet(RolloffFactor))
        m_source->setRolloffFactor(m_rolloffFactor);

    if (isSet(Pitch))
        m_source->setPitch(m_pitch);

    if (isSet(Velocity))
        m_source->setVelocity(m_velocity[0], m_velocity[1], m_velocity[2]);

    if (isSet(Ambient))    
        m_source->setAmbient(m_ambient);

    if (isSet(Relative))    
        m_source->setRelative(m_relative);

    int i = (m_is_set>>Play)&01;

    if (isSet(Play)) {
        if (m_play && m_enabled) 
            m_source->play();
        else if (m_pause)
            m_source->pause();
        else
            m_source->stop();
    }

    /// all changes has been set.
    setAll(false);
}

void SoundState::setEnable(bool flag)
{
    if (!flag && m_source.valid())
        m_source->stop();
    m_enabled = flag;
}

#ifdef max
#undef max
#endif
void SoundState::setAll(bool flag) 
{ 
    if (flag) 
        m_is_set = std::numeric_limits<unsigned int>::max();
    else m_is_set = 0; 
} //for(unsigned int i=0; i < m_is_set.size(); i++) m_is_set[i]=flag; }

/*------------------------------------------

* $Source: /cvsroot/osg al/osg AL/src/osg AL/SoundState.cpp,v $
* $Revision$ 
* $Date$
* $Author$ 
* $Locker:  $


* Description:

Author: Anders Backman
VRlab, Umeå University, 2002

* $Log: SoundState.cpp,v $
* Revision 1.9  2005/11/02 22:08:11  vr-anders
* Fixed bug in SoundState::allocateSource(), added call to apply() so that all fields are updated.
*
* Revision 1.8  2005/05/27 07:21:36  vr-anders
*   Alberto Jaspe (videaLAB / University of La Corua)
*   - Several miscelaneus bugfixes.
*   - Added support for reading/writing from/to osg files.
*   - Important bugfixes in doppler issues.
*   - Using just one update frequency for listener and sources, in SoundManager.
*   - Using a state for clamp function for max velocites in both listener and sources,
*     disabled by default.
*   - Replaced the FilePath Manager for the osgDB support.
*   - Added a new example called osg alviewer, that acts like osgviewer, but with some
*     features oriented to treat with Sound Nodes.
*   - Compiled and tested with OSG 0.9.9.
*
* Revision 1.7  2005/01/31 10:49:53  vrlab
* *** empty log message ***
*
* Revision 1.6  2004/11/22 07:40:35  andersb
* *** empty log message ***
*
* Revision 1.5  2004/11/19 07:46:10  andersb
* *** empty log message ***
*
* Revision 1.4  2004/11/11 07:47:44  andersb
* Added a simple occlude method for occluding soundsources against geometry
*
* Revision 1.3  2004/04/20 12:26:11  andersb
* Added SoundRoot
*
* Revision 1.2  2004/03/03 07:50:52  andersb
* *** empty log message ***
*
* Revision 1.1.1.1  2004/03/02 07:20:58  andersb
* Initial Version of the osg AL library
*
--------------------------------------------*/
