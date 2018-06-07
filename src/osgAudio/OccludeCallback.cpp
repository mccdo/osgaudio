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

#include <iostream>

#include <osg/Version>
#if OSG_VERSION_LESS_THAN(3,7,0)
#include <osgUtil/IntersectVisitor>
#else
#include <osgUtil/LineSegmentIntersector>
#endif

#include <osgAudio/OccludeCallback.h>
#include <osgAudio/SoundState.h>
#include <osgAudio/Math.h>

using namespace osgAudio;


OccludeCallback::OccludeCallback(osg::Node *root) : m_root(root), m_sound_state(0), m_ear_distance(0.2), m_near_threshold(0.1f),
m_was_occluded(false), m_delay(10)
{
}

/// Here we set an empty node. This constructor is called by osg when reading a file,
/// and later will the real node will be set.
OccludeCallback::OccludeCallback() : m_root(new osg::Node()), m_sound_state(0), m_ear_distance(0.2), m_near_threshold(0.1f),
m_was_occluded(false), m_delay(10)
{
}

void OccludeCallback::operator()(double /*distance*/, osg::Node * /*occluder*/, bool left_occluded, bool /*right_occluded*/)
{
    osgAudio::SoundState *sound_state = m_sound_state;

    // Sound node is occluded by something
    if (left_occluded) {

        // Was it not occluded last frame?
        if (!m_was_occluded)
            m_start_tick = osg::Timer::instance()->tick(); // Then start timer

        // Linearly interpolate occlusion from 0 to max
        double dt = m_delay*osg::Timer::instance()->delta_s(m_start_tick, osg::Timer::instance()->tick());
        float scale = osgAudio::mix(1.0f, 0.0f, dt);
        sound_state->setOccludeScale(scale);
        m_sound_state->setOccluded(true);
    }
    else { // Is not occluded anymore

        // If occlusion is already shut of, do no more
        if (!sound_state->getOccluded())
            return;

        if (m_was_occluded) // Was it occluded last frame, then start timer
            m_start_tick = osg::Timer::instance()->tick();

        // Interpolate from max to 0 damping
        double dt = m_delay*osg::Timer::instance()->delta_s(m_start_tick, osg::Timer::instance()->tick());
        float scale = osgAudio::mix(0.0f, 0.99f, dt);
        sound_state->setOccludeScale(scale);

        // When enough time have passed, disable occlusion
        if (dt > 1/m_delay)
            m_sound_state->setOccluded(false);
    }
}

void OccludeCallback::apply(const osg::Matrix& listener_matrix, const osg::Vec3& sound_pos, osgAudio::SoundState* sound_state)
{
    m_sound_state = sound_state;

    osg::Node *occluder=0;
    double distance = 0;

    osg::Matrix m = listener_matrix.inverse(listener_matrix);

#if OSG_VERSION_LESS_THAN(3,7,0)
    // Now shoot a ray from the ear to the source and see if it hits anything.
    osgUtil::IntersectVisitor iv;
    osg::ref_ptr<osg::LineSegment> left = new osg::LineSegment;

    osg::Vec3 start(m.getTrans()), end(sound_pos);
    osg::Vec3 dir = end-start;
    distance = dir.length();
    dir /= distance;

    left->set(start,start+dir*distance);

    if(left->valid())
        iv.addLineSegment(left.get());

    m_root->accept(iv);
    bool occluded = false;
    if (iv.hits()) {
        osgUtil::IntersectVisitor::HitList::iterator hit = iv.getHitList( left.get() ).begin();

        osg::Vec3 hit_point = hit->getWorldIntersectPoint();
        osg::NodePath &np = hit->_nodePath;
#else
    osg::Vec3 start(m.getTrans()), end(sound_pos);
    osg::ref_ptr<osgUtil::LineSegmentIntersector> intersector = new osgUtil::LineSegmentIntersector(start, end);
    intersector->setIntersectionLimit(osgUtil::LineSegmentIntersector::LIMIT_NEAREST);
    osgUtil::IntersectionVisitor intersectVisitor(intersector.get());
    m_root->accept(intersectVisitor);
    bool occluded = false;
    if (intersector->containsIntersections()) {
        osgUtil::LineSegmentIntersector::Intersection hit = intersector->getFirstIntersection();
        osg::Vec3 hit_point = hit.getWorldIntersectPoint();
        osg::NodePath &np = hit.nodePath;
#endif
        double d = (hit_point - start).length();
        double diff = fabs(d - distance);

        if ( diff > m_near_threshold) {
            occluded = true;

            if (np.size())
                occluder = *(np.begin());

            this->operator ()(d, occluder, true, true);
        }
    }

    // If it is not occluded this frame but it was the previous, restore the state
    if (!occluded)
        this->operator ()(0, NULL, false, false);

    // save the state of the occlusion for next frame
    m_was_occluded = occluded;    
}

