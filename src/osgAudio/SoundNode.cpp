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

#include <iomanip>

#include <osg/Transform>

#include <osgUtil/CullVisitor>

#include <osgAudio/SoundNode.h>
#include <osgAudio/SoundManager.h>

using namespace osg;
using namespace osgAudio;


SoundNode::SoundNode()
    :    
    osg::Node(), 
    m_sound_manager(SoundManager::instance()),
    m_last_time(0), 
    m_first_run(true), 
    m_last_traversal_number(0)
{
    setCullingActive(false);
}

SoundNode::SoundNode(SoundState *sound_state) 
    :    
    osg::Node(), 
    m_sound_state(sound_state),
    m_sound_manager(SoundManager::instance()),
    m_last_time(0), 
    m_first_run(true)
{
    setCullingActive(false);
}

SoundNode::SoundNode(SoundState *sound_state, SoundManager *sound_manager) 
:    osg::Node(), m_sound_state(sound_state),
m_sound_manager(sound_manager),
m_last_time(0), m_first_run(true)
{
    setCullingActive(false);
}


SoundNode & SoundNode::operator=(const SoundNode &node)
{ 
    if (this == &node) return *this; 

    m_sound_state = node.m_sound_state;
    m_sound_manager = node.m_sound_manager;
    m_last_time = node.m_last_time;
    m_first_run = node.m_first_run;
    return *this;
}



SoundNode::SoundNode(const SoundNode &copy, const osg::CopyOp &copyop)
:    osg::Node(copy, copyop)
{

    *this = copy;
}

void SoundNode::traverse(osg::NodeVisitor &nv)
{
    // continue only if the visitor actually is a cull visitor
    if (nv.getVisitorType() == osg::NodeVisitor::CULL_VISITOR)
    {
        // Make sure we only execute this once during this frame.
        // There could be two or more culls for stereo/multipipe...
        if (!m_sound_state.valid()) {
            // call the inherited method
            osg::notify(osg::DEBUG_INFO) << "SoundNode::traverse() No soundstate attached to soundnode" << std::endl;
            Node::traverse(nv);
            return;
        }
        if ( m_sound_state.valid() && nv.getTraversalNumber() != m_last_traversal_number && nv.getFrameStamp())
        {

            m_last_traversal_number = nv.getTraversalNumber();

            // retrieve the current time
            double t = nv.getFrameStamp()->getReferenceTime();
            double time = t - m_last_time;

            if(time >= m_sound_manager->getUpdateFrequency()) {

                osg::Matrix m;
                m = osg::computeLocalToWorld(nv.getNodePath());

                osg::Vec3 pos = m.getTrans();

                m_sound_state->setPosition(pos);

                //Calculate velocity
                osg::Vec3 velocity(0,0,0);

                if (m_first_run) {
                    m_first_run = false;
                    m_last_time = t;
                    m_last_pos = pos;
                }
                else {
                    velocity = pos - m_last_pos;
                    m_last_pos = pos;
                    m_last_time = t;
                    velocity /= time;
                }

                if(m_sound_manager->getClampVelocity()) {
                    float max_vel = m_sound_manager->getMaxVelocity();
                    float len = velocity.length();
                    if ( len > max_vel) {
                        velocity.normalize();
                        velocity *= max_vel;
                    }
                }

                m_sound_state->setVelocity(velocity);

                //Get new direction
                osg::Vec3 dir(0,1,0);

                dir = dir * m;
                dir.normalize();
                m_sound_state->setDirection(dir);      

                // Only do occlusion calculations if the sound is playing
                if (m_sound_state->getPlay() && m_occlude_callback.valid())
                    m_occlude_callback->apply(m_sound_manager->getListenerMatrix(), pos, m_sound_state.get());
            } // if
        }
    } // if cullvisitor

    // call the inherited method
    Node::traverse(nv);
}
