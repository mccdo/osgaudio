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


#include <osgDB/Registry>
#include <osgDB/Input>
#include <osgDB/Output>
#include <osgDB/FileUtils>

#include <osgAudio/FileStream.h>
#include <osgAudio/SoundState.h>
#include <osgAudio/SoundManager.h>

using namespace osgAudio;
using namespace osg;
using namespace osgDB;

// forward declare functions to use later.
bool SoundState_readLocalData(Object& obj, Input& fr);
bool SoundState_writeLocalData(const Object& obj, Output& fw);

// register the read and write functions with the osgDB::Registry.
RegisterDotOsgWrapperProxy SoundStateProxy
(
 new osgAudio::SoundState,
 "osgAudio::SoundState",
 "Object osgAudio::SoundState",
 &SoundState_readLocalData,
 &SoundState_writeLocalData
 );

// register the backward-compatible read and write functions with the osgDB::Registry.
RegisterDotOsgWrapperProxy SoundStateProxyBCAL
(
 new osgAudio::SoundState,
 "osgAL::SoundState",
 "Object osgAL::SoundState",
 &SoundState_readLocalData,
 NULL
 );

bool SoundState_readLocalData(osg::Object &obj, osgDB::Input &fr)
{

    SoundState &ss = static_cast<SoundState&>(obj);

    if (fr.matchSequence("streamFileName %s")) {
        osg::ref_ptr<osgAudio::Stream> s = SoundManager::instance()->getStream(fr[1].getStr());
        if(s.valid()) {
            ss.allocateSource();
            ss.setStream(s.get());
        }
        fr += 2;
    } else if (fr.matchSequence("fileName %s")) {
        osg::ref_ptr<osgAudio::Sample> s = SoundManager::instance()->getSample(fr[1].getStr());
        if(s.valid()) {
            ss.allocateSource();
            ss.setSample(s.get());
        }
        fr += 2;
    } else if (fr[0].matchWord("playing")) {
        if (fr[1].matchWord("TRUE")) {
            ss.setPlay(true);
        }
        else if (fr[1].matchWord("FALSE"))
            ss.setPlay(false);
        fr += 2;
    } else if (fr.matchSequence("gain %f")) {
        float f;
        fr[1].getFloat(f);
        ss.setGain(f);
        fr += 2;
    } else if (fr[0].matchWord("looping")) {
        if (fr[1].matchWord("TRUE"))
            ss.setLooping(true);
        else if (fr[1].matchWord("FALSE"))
            ss.setLooping(false);
        fr += 2;
    } else if (fr[0].matchWord("ambient")) {
        if (fr[1].matchWord("TRUE"))
            ss.setAmbient(true);
        else if (fr[1].matchWord("FALSE"))
            ss.setAmbient(false);
        fr += 2;
    } else if (fr[0].matchWord("relative")) {
        if (fr[1].matchWord("TRUE"))
            ss.setRelative(true);
        else if (fr[1].matchWord("FALSE"))
            ss.setRelative(false);
        fr += 2;
    } else if (fr.matchSequence("referenceDistance %f")) {
        float f;
        fr[1].getFloat(f);
        ss.setReferenceDistance(f);
        fr += 2;
    } else if (fr.matchSequence("maxDistance %f")) {
        float f;
        fr[1].getFloat(f);
        ss.setMaxDistance(f);
        fr += 2;
    } else if (fr.matchSequence("rolloffFactor %f")) {
        float f;
        fr[1].getFloat(f);
        ss.setRolloffFactor(f);
        fr += 2;
    } else if (fr.matchSequence("pitch %f")) {
        float f;
        fr[1].getFloat(f);
        ss.setPitch(f);
        fr += 2;
    } else  if (fr.matchSequence("occludeDampingFactor %f")) {
        float f;
        fr[1].getFloat(f);
        ss.setOccludeDampingFactor(f);
        fr += 2;
    } else if (fr[0].matchWord("enabled")) {
        if (fr[1].matchWord("TRUE"))
            ss.setEnable(true);
        else if (fr[1].matchWord("FALSE"))
            ss.setEnable(false);
        fr += 2;    
    } else  if (fr.matchSequence("soundCone %f %f %f")) {
        float f1, f2, f3;
        fr[1].getFloat(f1); fr[2].getFloat(f2); fr[3].getFloat(f3);
        ss.setSoundCone(f1, f2, f3);
        fr += 2;
    } else 
        return false;

    if(ss.hasSource())
        ss.apply();

    return true;
}

bool SoundState_writeLocalData(const Object& obj, Output& fw)
{
    const SoundState &ss = static_cast<const SoundState&>(obj);

    // Is there a sample associated with the stateSet?
    if (ss.getSample() != NULL)
        fw.indent() << "fileName \"" << ss.getSample()->getFilename() << "\"" << std::endl;

    // Is there a FileStream associated with the stateSet?
    if (ss.getStream() != NULL) {
        const osgAudio::FileStream *fs = dynamic_cast<const osgAudio::FileStream *> (ss.getStream());
        if (fs)
            fw.indent() << "streamFileName \"" << fs->getFilename() << "\"" << std::endl;
    }


    fw.indent() << "playing ";
    if (ss.isPlaying()) fw << "TRUE"<< std::endl;
    else fw << "FALSE"<< std::endl;

    fw.indent() << "enabled ";
    if (ss.getEnable()) fw << "TRUE"<< std::endl;
    else fw << "FALSE"<< std::endl;

    fw.indent() << "gain " << ss.getGain() << std::endl;
    fw.indent() << "looping ";
    if (ss.getLooping()) fw << "TRUE"<< std::endl;
    else fw << "FALSE"<< std::endl;
    fw.indent() << "ambient ";
    if (ss.getAmbient()) fw << "TRUE"<< std::endl;
    else fw << "FALSE"<< std::endl;
    fw.indent() << "relative ";
    if (ss.getRelative()) fw << "TRUE"<< std::endl;
    else fw << "FALSE"<< std::endl;    
    fw.indent() << "referenceDistance " << ss.getReferenceDistance() << std::endl;
    fw.indent() << "maxDistance " << ss.getMaxDistance() << std::endl;
    fw.indent() << "rolloffFactor " << ss.getRolloffFactor() << std::endl;
    fw.indent() << "pitch " << ss.getPitch() << std::endl;
    fw.indent() << "occludeDampingFactor " << ss.getOccludeDampingFactor() << std::endl;
    fw.indent() << "occludeScale " << ss.getOccludeScale() << std::endl;

    // If outer and inner angle are 360º, it is a omnidirectional source, so
    // nothing about directional is written
    if (ss.getInnerAngle()<360 && ss.getOuterAngle()<360) {
        fw.indent() << "soundCone " << ss.getInnerAngle() << 
            " " << ss.getOuterAngle() << " " << ss.getOuterGain() << std::endl;
    }

    return true;
}
