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

#include <osgAudio/SoundUpdateCB.h>
#include <osg/Transform>
#include <osg/NodeVisitor>
#include <osg/io_utils>

#include <osgAudio/SoundManager.h>

#include <iomanip>


using namespace osg;
using namespace osgAudio;


SoundUpdateCB::SoundUpdateCB()
    : 
    osg::NodeCallback(),
    m_sound_manager(SoundManager::instance()),
    m_last_time(0),
    m_first_run(true), 
    m_last_traversal_number(0)
{
}

SoundUpdateCB::SoundUpdateCB(SoundState *sound_state) 
    : 
    osg::NodeCallback(),
    m_sound_state(sound_state),
    m_sound_manager(SoundManager::instance()),
    m_last_time(0),
    m_first_run(true)
{
}

SoundUpdateCB::SoundUpdateCB(SoundState *sound_state, SoundManager *sound_manager) 
  : osg::NodeCallback(),
    m_sound_state(sound_state),
    m_sound_manager(sound_manager),
    m_last_time(0),
    m_first_run(true)
{
}

SoundUpdateCB::SoundUpdateCB(const SoundUpdateCB &copy, const osg::CopyOp &copyop)
  : osg::NodeCallback(copy, copyop)
{
    *this = copy;
}

SoundUpdateCB & SoundUpdateCB::operator=(const SoundUpdateCB &node)
{ 
    if (this == &node)
        return( *this );

    m_sound_state = node.m_sound_state;
    m_sound_manager = node.m_sound_manager;
    m_last_time = node.m_last_time;
    m_first_run = node.m_first_run;
    return( *this );
}


void
SoundUpdateCB::operator()( osg::Node* node, osg::NodeVisitor* nv )
{
    const osg::FrameStamp* fs( nv->getFrameStamp() );
    if( !m_sound_state.valid() || ( fs == NULL ) )
    {
        // Early exit.
        osg::notify(osg::DEBUG_INFO) << "SoundUpdateCB::operator()() No SoundState attached, or invalid FrameStamp." << std::endl;
        traverse( node, nv );
        return;
    }

    const double t( fs->getReferenceTime() );
    const double time( t - m_last_time );
    if(time >= m_sound_manager->getUpdateFrequency())
    {
        const osg::Matrix m( osg::computeLocalToWorld( nv->getNodePath() ) );
        const osg::Vec3 pos = m.getTrans();
        m_sound_state->setPosition(pos);

        //Calculate velocity
        osg::Vec3 velocity(0,0,0);
        if (m_first_run)
        {
            m_first_run = false;
            m_last_time = t;
            m_last_pos = pos;
        }
        else
        {
            velocity = pos - m_last_pos;
            m_last_pos = pos;
            m_last_time = t;
            velocity /= time;
        }

        if(m_sound_manager->getClampVelocity())
        {
            float max_vel = m_sound_manager->getMaxVelocity();
            float len = velocity.length();
            if ( len > max_vel)
            {
                velocity.normalize();
                velocity *= max_vel;
            }
        }
        m_sound_state->setVelocity(velocity);

        //Get new direction
        osg::Vec3 dir = osg::Vec3( 0., 1., 0. ) * m;
        dir.normalize();
        m_sound_state->setDirection(dir);

        // Only do occlusion calculations if the sound is playing
        if (m_sound_state->getPlay() && m_occlude_callback.valid())
            m_occlude_callback->apply(m_sound_manager->getListenerMatrix(), pos, m_sound_state.get());
    } // if time to update

    traverse( node, nv );
}
