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
#ifndef OSGAUDIO_SOUNDROOT_H
#define OSGAUDIO_SOUNDROOT_H 1


#include <osgAudio/Export.h>

#include <osg/ref_ptr>
#include <osg/Object>
#include <osg/Node>
#include <osg/NodeVisitor>
#include <osg/CopyOp>
#include <osg/Vec3>
#include <osg/Matrix>

namespace osgAudio
{


    /// A node that updates the transformation of the listener. Should be traversed before any SoundNode nodes
    /**
    This class updates the internal state of the osgAudio::SoundManager during cull traversal from the current 
    modelView matrix.
    The cull traversal for this node should be called after any changes are made to the SoundManager (added
    SoundState events or such).   
    */
    class OSGAUDIO_EXPORT SoundRoot: public osg::Node {
    public:
        /// Default constructor
        SoundRoot();

        META_Node(osgAudio, SoundRoot);

        /*!
        The Camera's view matrix is used to position the Listener.
        SoundRoot::update() queries the camera during update to
        obtain the view and update the Listener position.
        If camera is NULL, SoundRoot sets the Listener position to 0,0,0.

        If you have multiple views, specify the Camera for the view you'd
        like to use to represent the Listener's audio experience.

        If you want to specify the Listener position explicitly from your
        application, then there is no need to set a Camera here. However, you
        will need to explicitly set the Listener position after the update
        traversal and before any cull traversals, like so:
            osgAudio::SoundManager::instance()->setListenerMatrix( m );
        */
        void setCamera( osg::Camera* cam );
        const osg::Camera* getCamera() const;

        void setUpdateEnable(bool flag) { m_update_enabled = flag; }
        bool getUpdateEnable()  const { return m_update_enabled; }

        /*!
        Called by the update callback during the update traversal.
        Sets the orientation/position and velocity of the Listener.
        */
        void update( osg::NodeVisitor* nv );


    protected:
        /// Destructor
        virtual ~SoundRoot() {}

        /// Copy constructor
        SoundRoot(const SoundRoot &copy, const osg::CopyOp &copyop = osg::CopyOp::SHALLOW_COPY);

        /// Assignment operator
        SoundRoot &operator=(const SoundRoot &node); 


        osg::ref_ptr< osg::Camera > _camera;

        double m_last_time;
        bool m_update_enabled;
    };

    // INLINE FUNCTIONS
} // namespace osgAudio


#endif //OSGAUDIO_SOUNDROOT_H
