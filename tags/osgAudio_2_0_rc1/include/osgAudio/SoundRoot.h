/* -*-c++-*- */
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
		Executed during traversal of the scenegraph.
		If the NodeVisitor is a CullVisitor the orientation/position and velocity of the
		listener is updated.
		*/
		virtual void traverse(osg::NodeVisitor &nv);

		void setUpdateEnable(bool flag) { m_update_enabled = flag; }
		bool getUpdateEnable()  const { return m_update_enabled; }


	protected:

		/// Destructor
		virtual ~SoundRoot() {}

		/// Copy constructor
		SoundRoot(const SoundRoot &copy, const osg::CopyOp &copyop = osg::CopyOp::SHALLOW_COPY);

		/// Assignment operator
		SoundRoot &operator=(const SoundRoot &node); 

	private:
		double m_last_time;
		double m_first_run;
		osg::Vec3 m_last_pos;
		int m_last_traversal_number;
		bool m_update_enabled;
	};

	// INLINE FUNCTIONS
} // namespace osgAudio


#endif //OSGAUDIO_SOUNDROOT_H
