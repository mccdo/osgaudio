/* -*-c++-*- $Id$ */
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

#include <osgAudio/OccludeCallback.h>
#include <osgAudio/SoundState.h>

#include <osgDB/Registry>
#include <osgDB/Input>
#include <osgDB/Output>
#include <osgDB/FileUtils>

using namespace osgAudio;
using namespace osg;
using namespace osgDB;

// forward declare functions to use later.
bool OccludeCallback_readLocalData(Object& obj, Input& fr);
bool OccludeCallback_writeLocalData(const Object& obj, Output& fw);

// register the read and write functions with the osgDB::Registry.
RegisterDotOsgWrapperProxy OccludeCallbackProxy
(
 new osgAudio::OccludeCallback,
 "osgAudio::OccludeCallback",
 "Object osgAudio::OccludeCallback",
 &OccludeCallback_readLocalData,
 &OccludeCallback_writeLocalData
 );

// register the backward-compatible read and write functions with the osgDB::Registry.
RegisterDotOsgWrapperProxy OccludeCallbackProxyBCAL
(
 new osgAudio::OccludeCallback,
 "osgAL::OccludeCallback",
 "Object osgAL::OccludeCallback",
 &OccludeCallback_readLocalData,
 NULL
 );

bool OccludeCallback_readLocalData(osg::Object &obj, osgDB::Input &fr)
{

    OccludeCallback &oc = static_cast<OccludeCallback&>(obj);

    Node *n = dynamic_cast<Node*>(fr.readObject());
    if(n != NULL)
        oc.setOccludingNode(n);

    if (fr.matchSequence("nearThreshold %f")) {
        float f;
        fr[1].getFloat(f);
        oc.setNearThreshold(f);
        fr += 2;
    } else 
        return false;

    return true;
}

bool OccludeCallback_writeLocalData(const Object& obj, Output& fw)
{
    const OccludeCallback &oc = static_cast<const OccludeCallback&>(obj);
    if(oc.getOccludingNode() != NULL) 
        fw.writeObject( *oc.getOccludingNode() );
    fw.indent() << "nearThreshold " << oc.getNearThreshold() << std::endl;
    return true;
}
