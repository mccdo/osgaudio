// $Id$
/**
 * osgAudio - OpenSceneGraph Audio Library
 * Copyright (C) 2010 AlphaPixel, LLC
 * based on a fork of:
 * Osg AL - OpenSceneGraph Audio Library
 * Copyright (C) 2004 VRlab, Ume� University
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
 */


#include <osg/NodeCallback>
#include <osg/Transform>
#include <osg/Notify>
#include <osg/Version>

#include <osgUtil/CullVisitor>

#include <osgAudio/SoundManager.h>
#include <osgAudio/SoundRoot.h>

using namespace osg;
using namespace osgAudio;


class SoundRootUpdate : public osg::NodeCallback
{
public:
    SoundRootUpdate( SoundRoot& sr )
      : _sr( sr )
    {
        osg::notify( osg::ALWAYS ) << "SoundRootUpdate: Constructor" << std::endl;
    }
    ~SoundRootUpdate()
    {}

    virtual void operator()( osg::Node* node, osg::NodeVisitor* nv )
    {
        _sr.update( nv );

        traverse( node, nv );
    }

protected:
    SoundRoot& _sr;
};


// Determine whether or not we are at least OSG 2.8
#if( ( (OPENSCENEGRAPH_MAJOR_VERSION * 100) + OPENSCENEGRAPH_MINOR_VERSION ) >= 208 )
#  define OSG280 1
#else
#  undef OSG280
#endif

SoundRoot::SoundRoot()
  : _camera( NULL ),
    m_last_time(0),
    m_update_enabled(true)
{
#ifdef OSG280
    addUpdateCallback( new SoundRootUpdate( *this ) );
#else
    // OSG version < 2.8.0. addUpdateCallback() doesn't exist.
    setUpdateCallback( new SoundRootUpdate( *this ) );
#endif

    setCullingActive(false);
}


SoundRoot & SoundRoot::operator=(const SoundRoot &node)
{ 
	if (this == &node) return *this; 

    _camera = node._camera;
	m_last_time = node.m_last_time;
    m_update_enabled = node.m_update_enabled;

	return *this;
}



SoundRoot::SoundRoot(const SoundRoot &copy, const osg::CopyOp &copyop)
  : osg::Node( copy, copyop )
{
	*this = copy;
}


void
SoundRoot::setCamera( osg::Camera* cam )
{
    _camera = cam;
}
const osg::Camera*
SoundRoot::getCamera() const
{
    return( _camera );
}


void SoundRoot::update( osg::NodeVisitor* nv )
{
    osg::notify( osg::DEBUG_INFO ) << "SoundRoot::update: operator(). Camera: " << getCamera() << std::endl;

    const double curr_time( nv->getFrameStamp()->getReferenceTime() );
    bool time_to_update( ( m_last_time == 0.0 ) ||
        ( curr_time - m_last_time >= SoundManager::instance()->getUpdateFrequency() ) );

    if (time_to_update && m_update_enabled) {
	    m_last_time = curr_time;
#ifdef _DEBUG
	    if (dynamic_cast<osgUtil::CullVisitor *>(&nv) != &nv) throw std::exception("Implementation error: should have a osgUtil::CullVisitor");
#endif

	    if( osgAudio::SoundManager::instance()->initialized())
        {
	        // Update the soundmanager (process queued sound states)
	        osgAudio::SoundManager::instance()->update();

	        osg::Matrixd m;
            if( getCamera() != NULL )
                m = getCamera()->getViewMatrix();

	        // Set the position/orientation of the listener
	        osgAudio::SoundManager::instance()->setListenerMatrix( m );
        }
    }
}

