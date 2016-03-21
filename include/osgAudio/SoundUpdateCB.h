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
#ifndef OSGAUDIO_SOUND_UPDATE_CB_H
#define OSGAUDIO_SOUND_UPDATE_CB_H 1


#include <osgAudio/Export.h>

#include <osg/ref_ptr>
#include <osg/observer_ptr>
#include <osg/Object>
#include <osg/Node>
#include <osg/NodeVisitor>
#include <osg/CopyOp>
#include <osg/Vec3>
#include <osg/Matrix>

#include <osgAudio/SoundState.h>
#include <osgAudio/OccludeCallback.h>



namespace osgAudio
{
    class SoundManager;

    /// Attache a sound to an OSG Node.
    /**
    This class is a thread-safe version of the deprecated SoundNode. It supports
    velocity, direction, and OccluderCallback for occluded sounds.
    */
    class OSGAUDIO_EXPORT SoundUpdateCB: public osg::NodeCallback {
    public:

        SoundUpdateCB();
        SoundUpdateCB( osgAudio::SoundState* sound_state );
        SoundUpdateCB( osgAudio::SoundState* sound_state, osgAudio::SoundManager* sound_manager );
        SoundUpdateCB( const SoundUpdateCB& copy, const osg::CopyOp &copyop = osg::CopyOp::SHALLOW_COPY );

        META_Object( osgAudio, SoundUpdateCB );

        virtual void operator()( osg::Node* node, osg::NodeVisitor* nv );

        void setSoundState(SoundState *sound_state) { m_sound_state = sound_state; }
        SoundState *getSoundState() { return m_sound_state.get(); }
        const SoundState *getSoundState() const { return m_sound_state.get(); }    

        void setOccludeCallback(OccludeCallback *cb) { m_occlude_callback = cb; }
        OccludeCallback *getOccludeCallback() { return m_occlude_callback.get(); }  
        const OccludeCallback *getOccludeCallback() const { return m_occlude_callback.get(); }  

    protected:
        virtual ~SoundUpdateCB() {}

        osgAudio::SoundUpdateCB& operator=( const osgAudio::SoundUpdateCB &cb ); 

        /*!@Todo: The SoundState should be referenced with a ref_ptr,
        This currently causes problem, as the soundstate, and therefore any referenced soundsources
        are held, until this node is deleted. Which can be by OpenSceneGraph model cache. The time of this event can
        be late, outside the scope of main. This causes problems with OpenAL, which for some reason reports an invalid
        context during the deletion of that source. Therefore, just an ordinary pointer. Memory is properly
        deallocated by the SoundManager, so its not really a problem.
        */
        // PEM Use regular ref_ptr and assume app will remove these prior to shutdown.
        osg::ref_ptr< osgAudio::SoundState > m_sound_state;
        //osg::ref_ptr<SoundState> m_sound_state;        // Sukender: I'm testing it with OpenAL-Soft and OSG 2.7+...

        osgAudio::SoundManager *m_sound_manager;

        osg::ref_ptr< osgAudio::OccludeCallback > m_occlude_callback;

        double m_last_time;
        bool m_first_run;
        osg::Vec3 m_last_pos;
        int m_last_traversal_number;
    };

// namespace osgAudio
}

// OSGAUDIO_SOUND_UPDATE_CB_H
#endif
