/* -*-c++-*- $Id: osgaudioviewer.cpp,v 1.2 */
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

#include <osg/DeleteHandler>
#include <osg/CoordinateSystemNode>
#include <osgDB/ReadFile>
#include <osgUtil/Optimizer>
#include <osgViewer/Viewer>

#include <osgAudio/SoundManager.h>
#include <osgAudio/SoundRoot.h>

#define OSGAUDIO_DEFAULT_MAX_SOUNDSOURCES_ALLOWED 16
#define OSGAUDIO_DEFAULT_GAIN 1.0
#define OSGAUDIO_DEFAULT_DOPPLER_FACTOR 1.0
#define OSGAUDIO_DEFAULT_DISTANCE_MODEL osgAudio::InverseDistance

class FindSoundRootNodesVisitor : public osg::NodeVisitor {

public :
    typedef enum {
        SEARCH,
        SEARCH_AND_DESTROY
    } TMode ;

    FindSoundRootNodesVisitor() : osg::NodeVisitor(TRAVERSE_ALL_CHILDREN)
    { _found_count = 0; _mode = SEARCH; }

    virtual void apply(osg::Group &group ) {
        for (unsigned int i = 0; i<group.getNumChildren(); ) {
            if( dynamic_cast<osgAudio::SoundRoot*>(group.getChild(i)) && _mode==SEARCH_AND_DESTROY ) {
                group.removeChild(i);
            } else {
                if (dynamic_cast<osgAudio::SoundRoot*>(group.getChild(i)) && _mode==SEARCH)
                    _found_count++;

                group.getChild(i)->accept(*this);
                i++;
            }
        }
    }
    unsigned getHits() { return _found_count; }
    void setMode(TMode t) { _mode = t; }

private :
    osgAudio::SoundRoot *_found_node;
    unsigned _found_count;
    TMode _mode;
};


int main( int argc, char **argv )
{
    // use an ArgumentParser object to manage the program arguments.
    osg::ArgumentParser arguments(&argc,argv);

    // set up the usage document, in case we need to print out how to use this program.
    arguments.getApplicationUsage()->setApplicationName(arguments.getApplicationName());
    arguments.getApplicationUsage()->setDescription(arguments.getApplicationName()+" is the a modified version of standard OpenSceneGraph example which loads and visualises 3d models and sounds.");
    arguments.getApplicationUsage()->setCommandLineUsage(arguments.getApplicationName()+" [options] filename ...");
    arguments.getApplicationUsage()->addCommandLineOption("-h or --help","Display command line paramters");
    arguments.getApplicationUsage()->addCommandLineOption("--help-env","Display environmental variables available");
    arguments.getApplicationUsage()->addCommandLineOption("--help-keys","Display keyboard & mouse bindings available");
    arguments.getApplicationUsage()->addCommandLineOption("--help-all","Display all command line, env vars and keyboard & mouse bindigs.");
    arguments.getApplicationUsage()->addCommandLineOption("--maxsounds <n>","Sets the maximum number of sounds allowed.");
    arguments.getApplicationUsage()->addCommandLineOption("--gain <n>","Sets the global gain (volume)");
    arguments.getApplicationUsage()->addCommandLineOption("--dopplerfactor <n>","Sets the doppler factor");
    arguments.getApplicationUsage()->addCommandLineOption("--distancemodel <mode>", "NONE | INVERSE_DISTANCE | INVERSE_DISTANCE_CLAMPED | LINEAR");

    // construct the viewer.
    osgViewer::Viewer viewer(arguments);

    // get details on keyboard and mouse bindings used by the viewer.
    viewer.getUsage(*arguments.getApplicationUsage());

    // if user request help write it out to cout.
    bool helpAll = arguments.read("--help-all");
    unsigned int helpType = ((helpAll || arguments.read("-h") || arguments.read("--help"))? osg::ApplicationUsage::COMMAND_LINE_OPTION : 0 ) |
        ((helpAll ||  arguments.read("--help-env"))? osg::ApplicationUsage::ENVIRONMENTAL_VARIABLE : 0 ) |
        ((helpAll ||  arguments.read("--help-keys"))? osg::ApplicationUsage::KEYBOARD_MOUSE_BINDING : 0 );
    if (helpType)
    {
        arguments.getApplicationUsage()->write(std::cout, helpType);
        return 1;
    }

    // Parsing the sound-related options

    unsigned int maxSounds = OSGAUDIO_DEFAULT_MAX_SOUNDSOURCES_ALLOWED;
    arguments.read("--maxsounds", maxSounds);

    float gain = OSGAUDIO_DEFAULT_GAIN;
    arguments.read("--gain", gain);

    float dopplerFactor = OSGAUDIO_DEFAULT_DOPPLER_FACTOR;
    arguments.read("--dopplerfactor", dopplerFactor);

    osgAudio::DistanceModel distanceModel = OSGAUDIO_DEFAULT_DISTANCE_MODEL;
    std::string s_model;
#undef None // Someone in Linux is defining it 8|
    if (arguments.read("--distancemodel", s_model)) {
        if (s_model == "NONE")
            distanceModel = osgAudio::None;
        else if (s_model == "INVERSE_DISTANCE")
            distanceModel = osgAudio::InverseDistance;
        else if (s_model == "INVERSE_DISTANCE_CLAMPED")
            distanceModel = osgAudio::InverseDistanceClamped;    
        else if (s_model == "LINEAR")
            distanceModel = osgAudio::Linear;    
        else
            osg::notify(osg::WARN) << "WARNING: I do not understand that -distancemodel parameter" << std::endl;
    }

    // report any errors if they have occured when parsing the program aguments.
    if (arguments.errors())
    {
        arguments.writeErrorMessages(std::cout);
        return 1;
    }

    if (arguments.argc()<=1)
    {
        arguments.getApplicationUsage()->write(std::cout,osg::ApplicationUsage::COMMAND_LINE_OPTION);
        return 1;
    }

    try {

        // here we init the SoundManager
        osgAudio::SoundManager::instance()->init(maxSounds);
        osgAudio::SoundManager::instance()->getEnvironment()->setGain(gain);
        osgAudio::SoundManager::instance()->getEnvironment()->setDopplerFactor(dopplerFactor);
        osgAudio::SoundManager::instance()->getEnvironment()->setDistanceModel(distanceModel);

        osg::Timer_t start_tick = osg::Timer::instance()->tick();

        // read the scene from the list of file specified commandline args.
        osg::ref_ptr<osg::Node> loadedModel = osgDB::readNodeFiles(arguments);

        // if no model has been successfully loaded report failure.
        if (!loadedModel) 
        {
            std::cout << arguments.getApplicationName() <<": No data loaded" << std::endl;
            return 1;
        }

        // any option left unread are converted into errors to write out later.
        arguments.reportRemainingOptionsAsUnrecognized();

        // report any errors if they have occured when parsing the program aguments.
        if (arguments.errors())
        {
            arguments.writeErrorMessages(std::cout);
        }

        osg::Timer_t end_tick = osg::Timer::instance()->tick();

        std::cout << "Time to load = "<<osg::Timer::instance()->delta_s(start_tick,end_tick)<<std::endl;

        // optimize the scene graph, remove rendundent nodes and state etc.
        osgUtil::Optimizer optimizer;
        optimizer.optimize(loadedModel.get());

        // *********** Sound Root Node
        // Used for update the listener position and direction

        // First, here we find out how many sound root nodes there are
        FindSoundRootNodesVisitor fsrnv;
        fsrnv.setMode(FindSoundRootNodesVisitor::SEARCH);
        loadedModel->accept(fsrnv);

        if (fsrnv.getHits() == 0) {
            // there is no osgAudio::SoundRoot to update the listener, so we insert one
            osg::ref_ptr<osgAudio::SoundRoot> soundRoot = new osgAudio::SoundRoot();
            soundRoot->setCamera( viewer.getCamera() );
            osg::ref_ptr<osg::Group> rootGroup = static_cast<osg::Group*>(loadedModel.get());
            rootGroup->addChild(soundRoot.get());
        } else if (fsrnv.getHits() == 1) {
            // there is one, so no problem
        } else if (fsrnv.getHits() > 1) {
            // there are more than one SoundRoots !!! This is no critical, but
            // no useful. So, we warn about it...
            osg::notify(osg::WARN) << "WARNING!: the loaded model(s) has more than one sound root." << std::endl;
            osg::notify(osg::WARN) << "          If you are using more than one model, maybe each one has its" << std::endl;
            osg::notify(osg::WARN) << "          own osgAudio::SoundRoot. I left in the loaded scene graph just one." << std::endl;
            // destroy all the SoundRoot nodes...
            fsrnv.setMode(FindSoundRootNodesVisitor::SEARCH_AND_DESTROY);
            loadedModel->accept(fsrnv);
            // and add just one SoundRoot node
            osg::ref_ptr<osgAudio::SoundRoot> soundRoot = new osgAudio::SoundRoot();
            soundRoot->setCamera( viewer.getCamera() );
            osg::ref_ptr<osg::Group> rootGroup = static_cast<osg::Group*>(loadedModel.get());
            rootGroup->addChild(soundRoot.get());
        }

        // pass the loaded scene graph to the viewer.
        viewer.setSceneData(loadedModel.get());

        // create the windows and run the threads.
        viewer.realize();

        viewer.run();
    } catch(std::runtime_error& e) {
        std::cerr << "Caught: " << e.what() << std::endl;
    }

    // Very important to call before end of main!
    if (osg::Referenced::getDeleteHandler())
        osg::Referenced::getDeleteHandler()->setNumFramesToRetainObjects(0);
    osgAudio::SoundManager::instance()->shutdown();

    return 0;
}

