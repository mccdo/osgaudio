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

#ifndef OSGAUDIO_OCCLUDECALLBACK_H
#define OSGAUDIO_OCCLUDECALLBACK_H 1

#include <osgAudio/Export.h>

#include <iostream>

#include <osg/Referenced>
#include <osg/Node>
#include <osg/Matrix>
#include <osg/Timer>

namespace osgAudio {

    class SoundState;


    /// Class to calculate occlusion between the listener and the soundsource
    /*!
    OccludeCallback is executed from a SoundUpdateCB during the update traversal (or the depracted
    SoundNode during cull).
    By analyzing if the line-of-sight is free by shooting a ray from the listener towards the position
    specified in the apply() call, 
    a damping factor is applied to the gain of the soundstate for the current sound node.

    Currently only one ray is shot from the center of the listener to the center of the soundsource, instead
    of one per ear.

    The operator() method is executed both when the source is occluded as well as it is not.
    operator() implements the actual occlusion functionality.

    The default one, interpolates the damping over a short timeperiod trying to avoid clicks...

    */
    class OSGAUDIO_EXPORT OccludeCallback : public osg::Object {
    public:

        /*!
        Constructor.
        \param root - The intersection ray will be shot at all root's children.
        */
        OccludeCallback(osg::Node *root);

        /*!
        Constructor without args.
        */
        OccludeCallback();

        // Implementation of virtual functions of osg::Object
        virtual osg::Object* cloneType() const { return new OccludeCallback(); }
        virtual osg::Object* clone(const osg::CopyOp&) const {
            return new OccludeCallback(*this); 
        }
        virtual const char* libraryName() const { return "osgAudio"; }
        virtual const char* className() const { return "OccludeCallback"; }

        /// Returns the specified distance between the ears
        float getEarDistance() const { return m_ear_distance; }

        /// If the ray hit an object outside this distance from the sound node, it is culled
        void setNearThreshold(float t) { m_near_threshold = t; }

        ///
        float getNearThreshold() const { return m_near_threshold; }

        /// Set the node tree that will tested to possibly occlude the sound
        void setOccludingNode(osg::Node *root) {
            m_root = root;
        }

        /// Get the node tree that will tested to possibly occlude the sound
        osg::Node * getOccludingNode() {
            return m_root.get();
        }

        /// Get the const node tree that will tested to possibly occlude the sound
        const osg::Node * getOccludingNode() const {
            return m_root.get();
        }     

    protected:
        virtual void operator()(double distance, osg::Node *occluder, bool left_occluded, bool right_occluded);

        SoundState *getSoundState() { return m_sound_state; }

    private:
        friend class SoundUpdateCB;
        friend class SoundNode;

        /*! Executed from SoundUpdateCB::operator() during update (or the deprecated
        SoundNode during cull).
        \param listener_matrix - contains the transformation matrix for the listener.
        \param sound_pos - Position of the current sound
        \param sound_state - The sound state to potentially occlude.
        */
        void apply(const osg::Matrix& listener_matrix, const osg::Vec3& sound_pos, osgAudio::SoundState *sound_state);

        // The node from which the intersect is performed
        osg::ref_ptr<osg::Node> m_root;
        SoundState* m_sound_state;
        float m_ear_distance;
        float m_near_threshold;

        // was the sound node occluded last frame?
        bool m_was_occluded;
        const double m_delay;    

        osg::Timer_t m_start_tick;


    };

} // Namespace osgAudio

#endif // OSGAUDIO_OCCLUDECALLBACK_H


