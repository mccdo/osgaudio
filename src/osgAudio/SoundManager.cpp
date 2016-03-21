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

#include <cassert>

#include <osg/Notify>
#include <osgDB/FileUtils>

#include <osgAudio/SoundManager.h>

using namespace osgAudio;

SoundManager::SoundStateFlyWeight::SoundStateFlyWeight(unsigned no_states)
{
    SoundState *state = 0;

    for(unsigned int i=0; i < no_states; i++) {
        state = new SoundState("");    
        m_sound_states.push_back(state);
        m_available_states.push(state);
    }
}

SoundState *SoundManager::SoundStateFlyWeight::getSoundState(SoundState *state)
{
    if (!m_available_states.size())
        return 0;

    SoundState *copy_state=m_available_states.top();
    m_available_states.pop();

    // Make it a copy of the given state
    *copy_state = *state;

    return copy_state;
}

void SoundManager:: SoundStateFlyWeight::releaseSoundState(SoundState *state)
{
    m_available_states.push(state);
}


SoundManager::SoundStateFlyWeight::~SoundStateFlyWeight()
{
    m_sound_states.clear();
}


SoundManager::SoundManager()
    :
    m_sound_state_FlyWeight(0),
    m_listener(0), 
    m_sound_environment(0),  
    m_initialized(false), 
    m_max_velocity(2),
    m_last_tick(0),
    m_first_run(true),
#ifdef WIN32
    m_clamp_velocity(false),
#else
    m_clamp_velocity(true),
#endif                   
    m_update_frequency(1/100.0)
{
    m_listener_direction = osg::Vec3(1,1,1);
}

void SoundManager::init( unsigned int num_soundsources, bool displayInitMsgs )
{
    // Alternate interface for init, allows app to turn on display of 
    // initialization messages to the console.
    init( num_soundsources, 343, displayInitMsgs );
}
void SoundManager::init(unsigned int num_soundsources, float sound_velocity, bool displayInitMsgs )
{

    if( !m_sound_environment )
    {
        m_sound_environment = osgAudio::AudioEnvironment::instance();
        m_sound_environment->init( displayInitMsgs );
    }

    if (!m_listener) {
        m_listener = new Listener;
        m_listener->select();
    }

    if (!m_soundsources.size()) {
        try {
            while(m_soundsources.size() < num_soundsources) {//for(unsigned int i=0; i < num_soundsources; i++)
                Source *source = new Source();
                m_soundsources.push_back(source);
                m_available_soundsources.push_back(source);
            }
        }
        catch(osgAudio::NameError & e) {
            osg::notify(osg::WARN) << "SoundManager::init() NameError: " << e.what() << std::endl;
        }
    }
    if (!m_soundsources.size())
        throw std::runtime_error("SoundManager::init(): Unable to create sufficient soundsources");

    m_sound_state_FlyWeight = new SoundStateFlyWeight(100);

    // Set the sound velocity
    m_sound_environment->setSoundVelocity(sound_velocity);

    m_initialized=true;
}

void SoundManager::shutdown()
{
    if (!m_initialized)
        return;

    SoundStateVector::iterator ssv;

    // Cleanup any leftover active soundstates before cleaningup the rest
    for(ssv = m_active_sound_states.begin(); ssv != m_active_sound_states.end(); ssv++) {
        (*ssv)->releaseSource();
        m_sound_state_FlyWeight->releaseSoundState((*ssv).get());    
    }

    m_sound_states.clear();
    m_active_sound_states.clear();

    while(!m_sound_state_queue.empty())
        m_sound_state_queue.pop();

    m_soundsources.clear();
    m_available_soundsources.clear();
    m_active_soundsources.clear();


    m_sample_cache.clear();
    m_stream_cache.clear();

#ifdef ENABLE_SUBSYSTEM_OPENAL
    //This is needed specifically for OpenAL because it requires an explicit
    //shutdown process
    m_sound_environment->shutdown();
#endif
    m_listener=0;
    m_sound_state_FlyWeight=0;
    m_sound_environment=0;

    m_initialized=false;
}

SoundManager::~SoundManager()
{

    // If we are still initalized, then someone has forgotten to call
    // shutdown() before we get here.
    // This is because we can't shut down openAL outside main() (it crashes in Windows).
    // This destructor is called outside main because it is a singleton.
    if (initialized()) {
        std::string msg;
        msg = "SoundManager::shutdown() should be called for the SoundManager before the destructor is called";
        osg::notify(osg::WARN) << "SoundManager::~SoundManager(): " << msg << std::endl;
        //throw std::runtime_error("SoundManager::~SoundManager(): " + msg);
    }
}

Source *SoundManager::getSource(unsigned int priority, bool registrate_as_active, int depth)
{

    // Check for eternal recursion
    if (depth > 1)
        throw std::runtime_error("SoundManager::getSource(): Internal error, I can't seem to get a free SoundSource, no matter how I try");

    // Is there a soundsource available?
    if (m_available_soundsources.size()) {
        SourceVector::iterator smi;

        //warning("getSource") << "Avail soundsources: " << m_available_soundsources.size() << std::endl;

        // Get the last soundsource (try to work at the end of the vector always
        smi = m_available_soundsources.end();
        smi--;
        Source *source = (*smi).get();
        m_available_soundsources.pop_back();

        //warning("getSource") << "After get: Avail soundsources: " << m_available_soundsources.size() << std::endl;

        // Should we register this as an active soundsource (which can be reallocated by someone requesting it with higher priority)
        if (registrate_as_active)
            m_active_soundsources.push_back( std::make_pair(priority, source) );

        //warning("getSource") << "Active sound sources: " << m_active_soundsources.size() << std::endl;
        //warning("getSource") << "Source: " << source << std::endl;
        return source;
    }

    // No soundsources available. Is there a non-looping soundsource with lower priority?
    ActiveSourceVector::iterator smi;

    for(smi=m_active_soundsources.begin(); smi != m_active_soundsources.end(); smi++) {
        if (smi->first < priority && !(smi->second)->isLooping()) {

            // Stop the Source
            smi->second->stop();

            // Go through all the active SoundStates and remove all with stopped Sources (this one goes...)
            update();

            //warning("getSource") << "No source avail got one, Active sound sources: " << m_active_soundsources.size() << std::endl;     
            // Now as there shold be one source available, let's get that one by doing a recursive call to this method
            return getSource(priority, registrate_as_active, depth++);

        }
    }
    return 0;
}

bool SoundManager::pushSoundEvent(SoundState *state, unsigned int priority)
{

    assert(state && "Invalid null SoundState pointer");

    // Do not push disabled soundevents
    if (!state->getEnable())
        return false;

    if (state->getLooping())
        throw std::runtime_error("SoundManager::pushSoundEvent: Cannot push a looping sound as a sound event, try to allocate source instead");

    SoundState *copy_state = m_sound_state_FlyWeight->getSoundState(state);
    if (copy_state) {
        m_sound_state_queue.push(SoundStateQueue::value_type(priority, copy_state));
        return true;
    }
    return false;
}

SoundManager* SoundManager::instance()
{
    static SoundManager* s_SoundManager = new SoundManager();
    return s_SoundManager;
}


bool SoundManager::removeSoundState(osgAudio::SoundState *state)
{
    bool found = false;
    {
        SoundStateSet::iterator it = m_sound_states.find(state);
        if (it != m_sound_states.end()) {
            m_sound_states.erase(it);
            found = true;
        }
    }
    {
        SoundStateVector::iterator it = m_active_sound_states.begin();
        for(; it != m_active_sound_states.end();) {
            if ((*it).get() == state) {
                // Remove it from the map
                it = m_active_sound_states.erase(it);
                found = true;
            }
            else {
                ++it;
            }
        }
    }

    return found;
}

bool SoundManager::removeSoundState(const std::string& id)
{
    for (SoundStateSet::iterator it = m_sound_states.begin();
        it != m_sound_states.end();
        ++it)
    {
        if ((*it)->getName() == id)
        {
            m_sound_states.erase(it);
            return true;
        };
    }
    return false;
}

void SoundManager::releaseSource(Source *source)
{
    if (!m_initialized)
        return;

    assert(source && "Invalid null pointer for Source");

    // Stop the source if it is playing
    source->stop();

    // Check if this source is one of the active sources, then remove it from the list
    // if active sources
    for(ActiveSourceVector::iterator smi=m_active_soundsources.begin(); smi != m_active_soundsources.end(); smi++){
        if (smi->second == source) {
            //resetSource(smi->second);
            m_active_soundsources.erase(smi);
            break;
        }
    }

    // Return it back to the pool of available sources
    // I should probably loop over to make sure we're not adding it twice
    m_available_soundsources.push_back(source);
    //warning("releaseSource") << "Avail sources:  " << m_available_soundsources.size() << std::endl;
}


// Move any non-playing soundsources to the list of available soundsources
void SoundManager::update()
{
    // Loop over list of all active SoundStates, if the associated source for the SoundState is 
    // finished playing, then move the SoundState back to the SoundStateFlyWeight.
    SoundStateVector::iterator ssv;

    for(SoundStateVector::iterator ssv = m_active_sound_states.begin(); ssv != m_active_sound_states.end(); ) {
        if (!(*ssv)->isActive()) {
            (*ssv)->releaseSource();
            m_sound_state_FlyWeight->releaseSoundState((*ssv).get());    
            ssv = m_active_sound_states.erase(ssv);
        }
        else{
            ssv++;
        }
    }

    processQueuedSoundStates();

    // some audio backends (FMOD) may need an explicit kick in the
    // head to update spatial positions (and other operations).
    // This is implemented in the AudioEnvironment::update() method.
    // Do this now.
    if( m_sound_environment )
    {
        m_sound_environment->update();
    }
}

void SoundManager::stopAllSources()
{
    for(SourceVector::iterator it=m_soundsources.begin();it != m_soundsources.end(); it++) {
        (*it)->stop();
    }
}

void SoundManager::processQueuedSoundStates()
{
    // Go through the queue until there are either no events left or no more soundsources.
    // For each SoundEvent, allocate a source and call apply for it with its soundsource
    // also, when getting it from the queue, add it to a list of active SoundStates.
    osg::ref_ptr< SoundState > state;

    while(m_available_soundsources.size() && m_sound_state_queue.size()) {
        state = m_sound_state_queue.top().getState();
        m_sound_state_queue.pop();
        Source *source = getSource(state->getPriority());
        state->setSource(source);
        state->apply();
        m_active_sound_states.push_back(state.get());
        //osg::notify(osg::INFO) << "Adding m_active_sound_states size: " << m_active_sound_states.size() << std::endl;
        //osg::notify(osg::INFO) << "Available sources size: " << m_available_soundsources.size() << std::endl;
    }
    
    if( !m_available_soundsources.size() ) {
        osg::notify(osg::WARN) << "SoundManager::processQueuedSoundStates(): There are no more sources to be allocated." << std::endl;
    }    
}

void SoundManager::resetSource(Source *source)
{
    source->setPitch();
    source->setGain(1);
    source->setMinMaxGain();
    source->setReferenceDistance();
    source->setRolloffFactor();
    source->setVelocity(0,0,0);
    source->stop();
    source->rewind();
}

Sample* SoundManager::getSample( const std::string& path, bool add_to_cache )
{

    osg::ref_ptr<Sample> sample;
    SampleMap::iterator smi;
    smi = m_sample_cache.find(path);
    bool found=false;
    if (smi != m_sample_cache.end()) {
        found = true;
        osg::notify(osg::INFO) << "SoundManager::getSample(): Found " << path << " in cache" << std::endl;
        sample = new Sample(*(smi->second));
    }
    else {
        osg::notify(osg::INFO) << "SoundManager::getSample(): Cache miss for " << path << ". Loading from file..." << std::endl;

        // Cache miss, load the file:
        std::string new_path = osgDB::findDataFile(path);
        if (new_path.empty()) {
            osg::notify(osg::WARN) << "SoundManager::getSample(): Unable to find requested file: " << path << std::endl;
            return 0;
        }

        sample = new Sample(new_path);

        // if the loading of the model was successful, store the sample in the cache
        // except if the user have indicated that it shouldn't be added to the cache
        if (sample.get() && add_to_cache) {
            m_sample_cache.insert(SampleMapValType(path, sample.get()));
        }
    }

    return sample.release();
}


Stream* SoundManager::getStream( const std::string& path, bool add_to_cache )
{
    FileStream *stream=0;
    StreamMap::iterator smi;
    smi = m_stream_cache.find(path);
    bool found=false;
    if (smi != m_stream_cache.end()) {
        found = true;
        osg::notify(osg::INFO) << path << " in cache" << std::endl;
        stream = new FileStream(*(smi->second));
    }
    else {

        osg::notify(osg::INFO) << "SoundManager::getStream: Cache miss for " << path << ". Loading from file..." << std::endl;

        try {
            // Cache miss, load the file:
            std::string new_path = osgDB::findDataFile(path);
            if (new_path.empty()) {
                osg::notify(osg::WARN) << "SoundManager::getStream(): Unable to find requested file: " << path << std::endl;

                return 0;
            }
            stream = new FileStream(new_path.c_str());
        }
        catch(osgAudio::FileError) {
            osg::ref_ptr<FileStream> s = stream;
        }
        // if the loading of the model was successful, store the model in the cache
        if (stream && add_to_cache) {
            m_stream_cache.insert(StreamMapValType(path, stream));
        }
    }

    return stream;
}

SoundState *SoundManager::findSoundState(const std::string& id)
{
    for (SoundStateSet::iterator it = m_sound_states.begin();
        it != m_sound_states.end();
        ++it)
    {
        if ((*it)->getName() == id)
            return it->get();
    }

    return 0;
}


void SoundManager::setListenerDirection(const osg::Vec3& dir)
{ 
    if (fabs(dir[0])- 0.01 < 0.01 ||
        fabs(dir[1])- 0.01 < 0.01 ||
        fabs(dir[2])- 0.01 < 0.01)
        throw std::runtime_error("SoundManager::setListenerDirection(): A value of zero (0) is set in one of the elements of the listenerDirection which is not valid");

    m_listener_direction = dir; 
}

void SoundManager::setListenerMatrix( const osg::Matrix& matrix)
{

    m_listener_matrix = matrix;

    osg::Vec3 eye_pos, up_vector, look_vector, center;

    osg::Matrixd m(matrix); // Just until getLookAt is const
    m.getLookAt(eye_pos, center, up_vector);

    look_vector = center - eye_pos;

    // Calculate velocity
    osg::Vec3 velocity(0,0,0);

    if (m_first_run) {
        m_first_run = false;
        m_last_tick = m_timer.tick();
        m_last_pos = eye_pos;
    }
    else {
        osg::Timer_t curr_tick = m_timer.tick();

        velocity = eye_pos - m_last_pos;
        m_last_pos = eye_pos;

        double time = m_timer.delta_s(m_last_tick, curr_tick);
        m_last_tick = curr_tick;
        velocity /= time;
    }

    const osg::Vec3& listener_direction = getListenerDirection();


    Listener *listener = getListener();
    listener->setPosition(eye_pos[0], eye_pos[1], eye_pos[2]);
    listener->setOrientation(
        look_vector[0], 
        look_vector[1], 
        look_vector[2], 
        up_vector[0]*listener_direction[0],   
        up_vector[1]*listener_direction[1],   
        up_vector[2]*listener_direction[2]);

    if(getClampVelocity()) {
        float max_vel = getMaxVelocity();
        float len = velocity.length();
        if ( len > max_vel) {
            velocity.normalize();
            velocity *= max_vel;
        }
    }

    listener->setVelocity(velocity[0], velocity[1], velocity[2] );
}


/*------------------------------------------

* $Source: /cvsroot/osg al/osg AL/src/osg AL/SoundManager.cpp,v $
* $Revision$ 
* $Date$
* $Author$ 
* $Locker:  $

* 

Author: Anders Backman
VRlab, Umeå University, 2004

* $Log: SoundManager.cpp,v $
* Revision 1.14  2005/12/08 12:41:16  vr-anders
* Added stopAllSources() method
*
* Revision 1.13  2005/11/14 08:49:12  vr-anders
* Moved processing of queued soundstates into a separate method.
*
* Revision 1.12  2005/05/27 07:21:36  vr-anders
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
* Revision 1.11  2005/01/31 10:49:53  vrlab
* *** empty log message ***
*
* Revision 1.10  2005/01/18 14:46:54  andersb
* Removed debug printouts
*
* Revision 1.9  2005/01/18 11:55:24  vrlab
* Added set/get UpdateFrequency and setUpdateEnable methods
*
* Revision 1.8  2005/01/13 07:25:56  andersb
* Fixed bug that caused crash in call to shutdown() if soundstates were still left in the
* active sound state queue.
*
* Revision 1.7  2004/11/19 07:46:10  andersb
* *** empty log message ***
*
* Revision 1.6  2004/11/11 07:47:44  andersb
* Added a simple occlude method for occluding soundsources against geometry
*
* Revision 1.5  2004/10/21 11:36:39  andersb
* Moved the clear of the soundsources vector.
*
* Revision 1.4  2004/04/20 12:26:11  andersb
* Added SoundRoot
*
* Revision 1.3  2004/03/05 09:42:32  andersb
* *** empty log message ***
*
* Revision 1.2  2004/03/03 07:50:52  andersb
* *** empty log message ***
*
* Revision 1.1.1.1  2004/03/02 07:20:58  andersb
* Initial Version of the osg AL library
*
--------------------------------------------*/
