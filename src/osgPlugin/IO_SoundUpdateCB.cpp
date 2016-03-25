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

#include <osgAudio/SoundUpdateCB.h>
#include <osgAudio/SoundManager.h>

#include <osgDB/Registry>
#include <osgDB/Input>
#include <osgDB/Output>
#include <osgDB/FileUtils>

using namespace osgAudio;
using namespace osg;
using namespace osgDB;

// forward declare functions to use later.
bool SoundUpdateCB_readLocalData(Object& obj, Input& fr);
bool SoundUpdateCB_writeLocalData(const Object& obj, Output& fw);

// register the read and write functions with the osgDB::Registry.
RegisterDotOsgWrapperProxy SoundUpdateCBProxy
(
 new osgAudio::SoundUpdateCB,
 "osgAudio::SoundUpdateCB",
 "Object NodeCallback osgAudio::SoundNode",
 &SoundUpdateCB_readLocalData,
 &SoundUpdateCB_writeLocalData
 );

bool SoundUpdateCB_readLocalData(osg::Object &obj, osgDB::Input &fr)
{
    SoundUpdateCB* cb = dynamic_cast<SoundUpdateCB*>( &obj );

    /************************************************************************
    * Using this code, apps like osgviewer could use this kind of nodes and work,
    * but they will exit with crash !
    ************************************************************************/

    if(!SoundManager::instance()->initialized()) {
        osg::notify(osg::WARN) << "osgdb_osgAudio::SoundUpdateCB_readLocalData(): WARNING!!! ";
        osg::notify(osg::WARN) << "The osgAudio plugin is initializing the SoundManager. ";
        osg::notify(osg::WARN) << "So surely nobody will shutdown it, and I will exit with a crash!" << std::endl;
        osgAudio::SoundManager::instance()->init(16);
        osgAudio::SoundManager::instance()->getEnvironment()->setDistanceModel(osgAudio::InverseDistance);
        osgAudio::SoundManager::instance()->getEnvironment()->setDopplerFactor(1);
    }

    bool itAdvanced = false;

    SoundState *ss = dynamic_cast<SoundState*>(fr.readObject());
    if(ss != NULL) {
        cb->setSoundState(ss);
        itAdvanced = true;
    }

    OccludeCallback *oc = dynamic_cast<OccludeCallback*>(fr.readObject());
    if(oc != NULL) {
        cb->setOccludeCallback(oc);
        itAdvanced = true;
    }

    return itAdvanced;
}

bool SoundUpdateCB_writeLocalData(const Object& obj, Output& fw)
{
    const SoundUpdateCB* cb = dynamic_cast<const SoundUpdateCB*>( &obj );
    bool itAdvanced = false;

    if(cb->getSoundState() != NULL)  {
        fw.writeObject( *(cb->getSoundState()) );
        itAdvanced = true;
    }

    if(cb->getOccludeCallback() != NULL) {
        fw.writeObject( *(cb->getOccludeCallback()) );
        itAdvanced = true;
    }

    return itAdvanced;
}
