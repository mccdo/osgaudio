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

#include <osgAudio/SoundRoot.h>

#include <osgDB/Registry>
#include <osgDB/Input>
#include <osgDB/Output>
#include <osgDB/FileUtils>

#include <iostream>

using namespace osgAudio;
using namespace osg;
using namespace osgDB;

// forward declare functions to use later.
bool SoundRoot_readLocalData(Object& obj, Input& fr);
bool SoundRoot_writeLocalData(const Object& obj, Output& fw);

// register the read and write functions with the osgDB::Registry.
RegisterDotOsgWrapperProxy SoundRootProxy
(
 new osgAudio::SoundRoot,
 "osgAudio::SoundRoot",
 "Object Node osgAudio::SoundRoot",
 &SoundRoot_readLocalData,
 &SoundRoot_writeLocalData
 );

// register the backward-compatible read and write functions with the osgDB::Registry.
RegisterDotOsgWrapperProxy SoundRootProxyBCAL
(
 new osgAudio::SoundRoot,
 "osgAL::SoundRoot",
 "Object Node osgAL::SoundRoot",
 &SoundRoot_readLocalData,
 NULL
 );

bool SoundRoot_readLocalData(osg::Object &obj, osgDB::Input &fr)
{
    SoundRoot &sr = static_cast<SoundRoot&>(obj);

    if (fr[0].matchWord("updateEnable")) {
        if (fr[1].matchWord("TRUE"))
            sr.setUpdateEnable(true);
        else if (fr[1].matchWord("FALSE"))
            sr.setUpdateEnable(false);
        fr += 2;
    } else
        return false;

    return true;
}

bool SoundRoot_writeLocalData(const Object& obj, Output& fw)
{
    const SoundRoot &sr = static_cast<const SoundRoot&>(obj);

    fw.indent() << "updateEnable ";
    if (sr.getUpdateEnable()) fw << "TRUE"<< std::endl;
    else fw << "FALSE"<< std::endl;

    return true;
}
