/* -*-c++-*- $Id: osgaudioocclude.cpp */
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
#include <osg/Notify>
#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>
#include <osg/Geometry>
#include <osg/Geode>
#include <osg/ShapeDrawable>
#include <osgUtil/Optimizer>
#include <osgDB/Registry>
#include <osgDB/ReadFile>
#include <osgGA/TrackballManipulator>
#include <osgGA/FlightManipulator>
#include <osgGA/DriveManipulator>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgViewer/ViewerEventHandlers>
#include <osgViewer/Viewer>

#include <osgAudio/SoundNode.h>
#include <osgAudio/SoundRoot.h>
#include <osgAudio/SoundManager.h>
#include <osgAudio/SoundState.h>
#include <osgAudio/OccludeCallback.h>
#include <osgAudio/Version.h>
#include <osgAudio/SoundUpdateCB.h>

osg::PositionAttitudeTransform *createSound(const std::string& file, bool occlude, osg::Node *root, bool is_stream);

int main( int argc, char **argv )
{

    osg::notify(osg::WARN) << "\n\n" << osgAudio::getLibraryName() << " demo" << std::endl;
    osg::notify(osg::WARN) << "Version: " << osgAudio::getVersion() << "\n\n" << std::endl;

    osg::notify(osg::WARN) << "Demonstrates occluders" << std::endl;


    try {
        // use an ArgumentParser object to manage the program arguments.
        osg::ArgumentParser arguments(&argc,argv);

        // set up the usage document, in case we need to print out how to use this program.
        arguments.getApplicationUsage()->setDescription(arguments.getApplicationName()+" demonstrates the use of the osgAudio toolkit for spatial sound.");
        arguments.getApplicationUsage()->setCommandLineUsage(arguments.getApplicationName()+" [options] filename ...");
        arguments.getApplicationUsage()->addCommandLineOption("-h or --help","Display this information");

        // initialize the viewer.
        osgViewer::Viewer viewer(arguments);

        osg::ref_ptr<osgGA::KeySwitchMatrixManipulator> keyswitchManipulator = new osgGA::KeySwitchMatrixManipulator;
        keyswitchManipulator->addMatrixManipulator( '1', "Trackball", new osgGA::TrackballManipulator() );
        viewer.setCameraManipulator( keyswitchManipulator.get() );

        // add the window size toggle handler
        viewer.addEventHandler(new osgViewer::WindowSizeHandler);

        // add the stats handler
        viewer.addEventHandler(new osgViewer::StatsHandler);

        // add the help handler
        viewer.addEventHandler(new osgViewer::HelpHandler(arguments.getApplicationUsage()));

        // get details on keyboard and mouse bindings used by the viewer.
        viewer.getUsage(*arguments.getApplicationUsage());

        // if user request help write it out to cout.
        if (arguments.read("-h") || arguments.read("--help"))
        {
            arguments.getApplicationUsage()->write(std::cout);
            return 1;
        }

        // any option left unread are converted into errors to write out later.
        arguments.reportRemainingOptionsAsUnrecognized();

        arguments.getApplicationUsage()->addKeyboardMouseBinding("RETURN", "Play a sound");


        // report any errors if they have occured when parsing the program aguments.
        if (arguments.errors())
        {
            arguments.writeErrorMessages(std::cout);
            return 1;
        }

        // initialize the SoundManager before loading any files
        osgAudio::SoundManager::instance()->init(16);
        osgAudio::SoundManager::instance()->getEnvironment()->setDistanceModel(osgAudio::InverseDistance);
        osgAudio::SoundManager::instance()->getEnvironment()->setDopplerFactor(1);


        osg::ref_ptr<osg::Group> rootnode = new osg::Group;
        // load the nodes from the commandline arguments.
        osg::Node* model = osgDB::readNodeFiles(arguments); //createModel();
        if (!model)
        {
            osg::notify(osg::FATAL) << "Error loading models from commandline" << std::endl;
            return 1;
        }
        osg::ref_ptr<osg::PositionAttitudeTransform> loaded_transform = new osg::PositionAttitudeTransform;
        loaded_transform->addChild(model);
        rootnode->addChild(loaded_transform.get());


        // Create ONE (only one, otherwise the transformation of the listener and update for SoundManager will be
        // called several times, which is not catastrophic, but unnecessary) 
        // SoundRoot that will make sure the listener is updated and
        // to keep the internal state of the SoundManager updated
        // This could also be done manually, this is just a handy way of doing it.
        osg::ref_ptr<osgAudio::SoundRoot> sound_root = new osgAudio::SoundRoot;
        sound_root->setCamera( viewer.getCamera() );



        // The position in the scenegraph of this node is not important.
        // Just as long as the cull traversal should be called after any changes to the SoundManager are made.
        rootnode->addChild(sound_root.get());


        bool occlude = true;

        osg::ref_ptr<osg::PositionAttitudeTransform> sound_transform = createSound("a.wav", occlude, rootnode.get(), false);

        rootnode->addChild(sound_transform.get());



        // run optimization over the scene graph
        //osgUtil::Optimizer optimizer;
        //optimizer.optimize(rootnode.get());

        // set the scene to render
        viewer.setSceneData(rootnode.get());


        // create the windows and run the threads.
        viewer.realize();

        osg::Timer_t start = osg::Timer::instance()->tick();
        float rate=10; // degrees per second
        while( !viewer.done() )
        {
            osg::Timer_t now = osg::Timer::instance()->tick();
            double dt = osg::Timer::instance()->delta_s(start, now);
            double angle = rate*dt;

            osg::Quat quat;
            quat.makeRotate(osg::inDegrees(angle), osg::Vec3(0,0,1));

            loaded_transform->setAttitude(quat);

            // update the scene by traversing it with the the update visitor which will
            // call all node update callbacks and animations.

            // fire off the cull and draw traversals of the scene.
            viewer.frame();
        }
    }
    catch (std::exception& e) {
        osg::notify(osg::WARN) << "Caught: " << e.what() << std::endl;
    }
    // Very important to call this before end of main.
    // Otherwise OpenAL will do all sorts of strange things after end of main
    // in the destructor of soundmanager.
    if (osg::Referenced::getDeleteHandler()) {
        osg::Referenced::getDeleteHandler()->setNumFramesToRetainObjects(0);
        osg::Referenced::getDeleteHandler()->flushAll();
    }
    osgAudio::SoundManager::instance()->shutdown();

    return 0;

}

osg::PositionAttitudeTransform *createSound(const std::string& file, bool occlude, osg::Node *root, bool is_stream)
{
    // Create a sample, load a .wav file.
    bool add_to_cache = false;
    osg::ref_ptr<osgAudio::Stream> stream;
    osg::ref_ptr<osgAudio::Sample> sample;

    // Create a new soundstate, give it the name of the file we loaded.
    osg::ref_ptr<osgAudio::SoundState> sound_state = new osgAudio::SoundState(file);
    // Allocate a hardware soundsource to this soundstate (priority 10)
    sound_state->allocateSource(10, false);

    if (is_stream) {
        stream = osgAudio::SoundManager::instance()->getStream(file.c_str(), add_to_cache);
        osg::notify(osg::WARN) << "Loading stream: " << file << std::endl;
        sound_state->setStream(stream.get());
    }
    else {
        sample = osgAudio::SoundManager::instance()->getSample(file.c_str(), add_to_cache);
        osg::notify(osg::WARN) << "Loading sample: " << file << std::endl;
        sound_state->setSample(sample.get());

    }

    sound_state->setGain(1.0f);
    sound_state->setReferenceDistance(10);
    sound_state->setRolloffFactor(1);
    sound_state->setPlay(true);
    sound_state->setLooping(true);

    // Add the soundstate to the sound manager, so we can find it later on if we want to
    osgAudio::SoundManager::instance()->addSoundState(sound_state.get());

    osg::ref_ptr< osgAudio::SoundUpdateCB > soundCB = new osgAudio::SoundUpdateCB;
    soundCB->setSoundState( sound_state.get() );
    
    float radius = 0.5;
    if (occlude)
    {
        osgAudio::OccludeCallback* cb = new osgAudio::OccludeCallback(root);
        cb->setNearThreshold(radius*1.1);
        soundCB->setOccludeCallback( cb );
    }

    // Create a transformation node onto we will attach a soundnode
    osg::PositionAttitudeTransform *sound_transform = new osg::PositionAttitudeTransform;

    // Create a sphere so we can "see" the sound
    osg::ref_ptr<osg::Geode> geode = new osg::Geode;
    geode->setUpdateCallback( soundCB.get() );
    osg::TessellationHints* hints = new osg::TessellationHints;
    hints->setDetailRatio(0.5f);
    geode->addDrawable(new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(0.0f,0.0f,0.0f),radius),hints));
    sound_transform->addChild(geode.get());

    return sound_transform;
}
