// $Id$
/**
 * osgAudio - OpenSceneGraph Audio Library
 * Copyright (C) 2010 AlphaPixel, LLC
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
 */


#include <osg/Transform>

#include <osgUtil/CullVisitor>

#include <osgAudio/SoundManager.h>
#include <osgAudio/SoundRoot.h>

using namespace osg;
using namespace osgAudio;


SoundRoot::SoundRoot()
:    m_last_time(0), m_first_run(true), 
m_last_traversal_number(0),
m_update_enabled(true)
{
	setCullingActive(false);
}


SoundRoot & SoundRoot::operator=(const SoundRoot &node)
{ 
	if (this == &node) return *this; 

	m_last_time = node.m_last_time;
	m_first_run = node.m_first_run;
	return *this;
}



SoundRoot::SoundRoot(const SoundRoot &copy, const osg::CopyOp &copyop)
:    osg::Node(copy, copyop)
{

	*this = copy;
}

void SoundRoot::traverse(osg::NodeVisitor &nv)
{
	// continue only if the visitor actually is a cull visitor
	if (nv.getVisitorType() == osg::NodeVisitor::CULL_VISITOR) {

		// Make sure we only execute this once during this frame.
		// There could be two or more culls for stereo/multipipe...
		if ( nv.getTraversalNumber() != m_last_traversal_number && nv.getFrameStamp())
		{

			m_last_traversal_number = nv.getTraversalNumber();

			bool time_to_update = false;
			double curr_time = nv.getFrameStamp()->getReferenceTime();

			if (curr_time - m_last_time >= SoundManager::instance()->getUpdateFrequency()) {
				time_to_update = true;
			}

			if (time_to_update && m_update_enabled) {
				m_last_time = curr_time;
#ifdef _DEBUG
				if (dynamic_cast<osgUtil::CullVisitor *>(&nv) != &nv) throw std::exception("Implementation error: should have a osgUtil::CullVisitor");
#endif
				osgUtil::CullVisitor *cv = static_cast<osgUtil::CullVisitor *>(&nv);

				//osg::Matrix m = *cv->getModelViewMatrix();		// Wrong when the main camera is using RELATIVE_RF
				osg::Matrixd m = cv->getCurrentCamera()->getViewMatrix();

				if(osgAudio::SoundManager::instance()->initialized()) {

					// Update the soundmanager (process queued sound states)
					osgAudio::SoundManager::instance()->update();

					// Set the position/orientation of the listener
					osgAudio::SoundManager::instance()->setListenerMatrix(m);
				}
			}
		}

	} 

	// call the inherited method
	Node::traverse(nv);
}
