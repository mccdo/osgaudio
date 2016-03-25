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



#ifndef OSGAUDIO_SOUNDMANAGER_H
#define OSGAUDIO_SOUNDMANAGER_H 1

#include <osgAudio/Export.h>

#include <string>
#include <map>
#include <queue>
#include <functional>
#include <stack>
#include <set>

#include <osg/ref_ptr>
#include <osg/Timer>
#include <osg/Matrix>

#include <osgAudio/SoundState.h>
#include <osgAudio/AudioEnvironment.h>
#include <osgAudio/FileStream.h>


namespace osgAudio 
{


    /// A SoundManager handles the sound system.
    /*!
    This class initialises the Sound system. It also keeps track of loaded sound samples.
    It is based on the Singleton Design pattern.
    To manage a limited resource of Sound sources (which is usually around 32)
    a schema has to be set up.

    This class initially initializes sound sources and places them into a pool of available sources.
    Whenever someone calls allocateSource() the SoundManager tries to free a source
    and return a pointer to it. If there are no sources available it goes through the 
    active sources and tries to find one with a lower priority. If it still fails, an exception is
    thrown.
    Otherwise the found source is returned.

    The allocateSource() has an argument indicating if the source should be placed in the
    list of active sources. If this argument is false, the caller is responsible to
    call releaseSource() when finished using the source.

    SoundManager also handles SoundStates.
    Whenever pushSoundEvent() is called, a FlyWeight set of SoundStates is checked for a free SoundState.
    The argument of pushSoundEvent() is copied to the free SoundState and that SoundState
    is then pushed to the queue of waiting SoundStates.

    When SoundManager::update() is called, the queue of waiting SoundStates is checked and for each
    SoundState a Source is allocated.
    When the SoundManager realizes that a SoundState is not playing anymore, it is removed from
    the list of active SoundStates and put back in to the FlyWeight set of available SoundStates.

    This makes the pushSoundEvent method suitable for playing soundeffects.
    */
    class OSGAUDIO_EXPORT SoundManager
    {
    public:


        /// Return a pointer to the singleton object
        static SoundManager* instance( void );

        /*! Deallocates any allocated memory, this method have to be called prior to the end of main()
        OpenAL doesn't like destruction of sources outside the main() function.
        */
        virtual void shutdown();


        /// Returns true if the SoundManager is initialized
        bool initialized() { return m_initialized; }


        /*!
        Push a SoundState to the queue of waiting SoundStates
        When the update method later on is called, this queue is inquired and each
        waiting SoundEvent will be put in the active state and put to a list of active 
        SoundStates with a Source allocated.
        \param state - The state that will be pushed to the waiting queue
        \param priority - The priority of the state, 0 lowest
        */
        bool pushSoundEvent(SoundState *state, unsigned int priority=0);

        /*! 
        Return a pointer to a Sample.
        Each Sample will be stored in a cache with its associated path (if add_to_cache is true)
        Each call to getSample will first look in the cache and try to find the
        path there. If it can be found, it will return that Sample.
        Otherwise it will be loaded from disk.
        */
        osgAudio::Sample* getSample( const std::string& path, bool add_to_cache=true );

        /*! 
        Clear the sample cache with all loaded samples.
        */
        void clearSampleCache(void) {  m_sample_cache.clear(); }


        /*! 
        Return a pointer to a Stream
        Each Stream will be stored in a cache with its associated path.
        Each call to getStream will first look in the cache and try to find the
        path there. If it can be found, it will return that Stream.
        Otherwise it will be loaded from disk.
        */
        osgAudio::Stream* getStream( const std::string& path, bool add_to_cache=true );

        /*! 
        Clear the Stream cache with all loaded streams.
        */
        void clearStreamCache(void) {  m_stream_cache.clear(); }


        /*!
        Return a pointer to the SoundState with the name "id".
        \return Null if no matching SoundState can be found.
        */
        SoundState *findSoundState(const std::string& id);

        /*!
        Puts the source into the list of available sound sources
        */
        void releaseSource(osgAudio::Source *source);

        /// Initializes the SoundManager
        void init(unsigned int num_soundsources, float sound_velocity=343, bool displayInitMsgs=false );
        void init( unsigned int num_soundsources, bool displayInitMsgs );

        /*!
        */
        void update();

        /// For each soundstate in queue, allocate a soundsource and play it.
        void processQueuedSoundStates();

        /// Return a pointer to the listener
        osgAudio::Listener *getListener();

        /// Return a pointer to the Sound environment
        osgAudio::AudioEnvironment *getEnvironment();

        /*!
        Add a SoundState to the list of existing sound states
        */
        void addSoundState(osgAudio::SoundState *state) { m_sound_states.insert(state); }

        /// Removes the named sound state from the list of existing soundstates
        bool removeSoundState(const std::string& id);

        /*!
        Removes the sound state from the list of existing soundstates
        */
        bool removeSoundState(osgAudio::SoundState *state);


        /*!
        To make it possible to flip left and right ear for the listener this vector specified
        will be multiplied with the up vector every time the orientation of the listener is calculated
        The default value is (1, 1, 1). Make sure not to set any component to zero, this will be caught
        and an exception will be thrown.
        */
        void setListenerDirection(const osg::Vec3& dir); 

        const osg::Vec3& getListenerDirection() const { return m_listener_direction; }

        /// Set the transformation matrix for the listener
        void setListenerMatrix( const osg::Matrix& matrix);

        /// Return the current listener matrix
        const osg::Matrix &getListenerMatrix( ) const { return m_listener_matrix; }

        /*!
        Tries to find an available sound Source
        \param if mutual_use then the source is registered in the soundmanager as a source that can be
        reused by a call to getSource with higher priority.
        If on the other hand mutual_use is false, then it is allocated 
        \return Pointer to an available sound Source
        */
        osgAudio::Source *allocateSource(unsigned int priority, bool mutual_use=true) { return getSource(priority, mutual_use); }


        /// Set the maximum velocity used in Doppler calculation
        void setMaxVelocity(float vel) { m_max_velocity = (float)fabs(vel); }

        /// Returns the maximum velocity used in Doppler calculation
        float getMaxVelocity() const { return m_max_velocity; }

        /*!
        Returns the number of free and available SoundSources for allocation.
        Should be between 0 ... getNumSources()
        */
        unsigned int getNumAvailableSources() { return m_available_soundsources.size(); }

        /*!
        Returns the number of initialized SoundSources.
        Should be between 0...?
        */
        unsigned int getNumSources() { return m_soundsources.size(); }

        /*!
        Stop playing on all initialized SoundSources immediately.
        */
        void stopAllSources();

        /*!
        Returns the number of SoundSources currently in use.
        Should be between 0...getNumSources()
        */
        unsigned int getNumActiveSources() { return m_active_soundsources.size(); }

        /// Set if the velocity is clamped (using the MaxVelocity attribute)
        void setClampVelocity(bool c) { m_clamp_velocity = c; }

        /// Get true if the velocity is clamped (using the MaxVelocity attribute)
        bool getClampVelocity() { return m_clamp_velocity; }

        /// Get const true if the velocity is clamped (using the MaxVelocity attribute)
        const bool getClampVelocity() const { return m_clamp_velocity; }

        /*!
        Set the update frequency for both sources and listener, to update pos, dir and occlusions
        \param frequency Not actually a frequency (in Hz) but rather an interval in floating-point seconds.
        */
        void setUpdateFrequency( float frequency ) { 
            if(frequency < 0) m_update_frequency = 1/100.0;
            else m_update_frequency = frequency;
        }

        /// Get the update frequency for both sources and listener, to update pos, dir and occlusions
        float getUpdateFrequency() { return m_update_frequency; }

        /// Get const the update frequency for both sources and listener, to update pos, dir and occlusions
        const float getUpdateFrequency() const { return m_update_frequency; }  

    private:
        osgAudio::Source *getSource(unsigned int priority, bool registrate_as_active=true, int depth=0);

        /// Destructor
        ~SoundManager();

        /// Default Constructor
        SoundManager();


        /// Class that handles all the soundstates. See FlyWeight Design pattern
        class SoundStateFlyWeight : public osg::Referenced {
        public:
            SoundStateFlyWeight(unsigned int no_states);
            virtual ~SoundStateFlyWeight();

            SoundState *getSoundState(SoundState *state);
            void releaseSoundState(SoundState *state);
            void clean() { m_sound_states.resize(0); }
        private:

            std::vector< osg::ref_ptr<SoundState> > m_sound_states;
            std::stack< SoundState *> m_available_states;
        };

        osg::ref_ptr<SoundStateFlyWeight> m_sound_state_FlyWeight;

        void resetSource(osgAudio::Source *source);

        typedef std::map<std::string, osg::ref_ptr<osgAudio::Sample> > SampleMap;
        typedef SampleMap::iterator SampleMapIterator;
        typedef SampleMap::value_type SampleMapValType;

        SampleMap m_sample_cache;

        typedef std::map<std::string, osg::ref_ptr<osgAudio::FileStream> > StreamMap;
        typedef StreamMap::iterator StreamMapIterator;
        typedef StreamMap::value_type StreamMapValType;

        StreamMap m_stream_cache;

        osg::ref_ptr<osgAudio::Listener> m_listener;
        osgAudio::AudioEnvironment* m_sound_environment;


        typedef std::vector<osg::ref_ptr<osgAudio::Source> > SourceVector;
        typedef std::vector<std::pair< unsigned int, osg::ref_ptr<osgAudio::Source> > > ActiveSourceVector;
        ActiveSourceVector m_active_soundsources; 
        SourceVector m_soundsources; 

        // A matrix containing the position and orientation of the listener
        osg::Matrix m_listener_matrix;  

        SourceVector m_available_soundsources; 

        typedef std::set<osg::ref_ptr<SoundState> > SoundStateSet;
        SoundStateSet m_sound_states;

        class SoundStateQueueItem {
        public:
            SoundStateQueueItem(unsigned int prio, SoundState *state) : m_prio(prio), m_state(state) {}
            bool operator <(const SoundStateQueueItem& item) const { return  m_prio <  item.m_prio; }
            bool operator ==(const SoundStateQueueItem& item) { return  m_prio == item.m_prio; }
            SoundState *getState() const { return m_state; }
            unsigned int getPriority() { return m_prio; }
        private:
            unsigned int m_prio;
            SoundState *m_state;
        };

        typedef std::priority_queue<SoundStateQueueItem> SoundStateQueue;
        SoundStateQueue m_sound_state_queue;

        typedef std::vector<osg::ref_ptr<SoundState> > SoundStateVector;
        SoundStateVector m_active_sound_states;

        bool m_initialized;

        osg::Vec3 m_last_pos;
        float m_max_velocity;
        osg::Timer m_timer;
        osg::Timer_t m_last_tick;
        bool m_first_run;
        bool m_clamp_velocity;
        float m_update_frequency;
        osg::Vec3 m_listener_direction;

    };


    inline osgAudio::Listener *SoundManager::getListener()
    { 
        if (!m_initialized) {
            throw std::runtime_error("SoundManager::getListener(): No sound system initialized");
            return 0;
        }
        return m_listener.get(); 
    }

    inline osgAudio::AudioEnvironment* SoundManager::getEnvironment() 
    { 
        if (!m_initialized) {
            throw std::runtime_error("SoundManager::getEnvironment(): Soundsystem is not initialized");
            return 0;
        }
        return m_sound_environment; 
    }

} // namespace osgAudio

#define g_SoundManager osgAudio::SoundManager::instance()
#endif // OSGAUDIO_SOUNDMANAGER_H
