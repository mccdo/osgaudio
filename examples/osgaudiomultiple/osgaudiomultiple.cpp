/* -*-c++-*- $Id: osgaudiomultiple.cpp */
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
#include <osg/Version>
#include <osgUtil/Optimizer>
#include <osgDB/Registry>
#include <osgDB/ReadFile>
#include <osgGA/TrackballManipulator>
#include <osgGA/FlightManipulator>
#include <osgGA/DriveManipulator>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgViewer/ViewerEventHandlers>
#include <osgViewer/Viewer>

//#define USE_SOUNDNODE 1
#ifdef USE_SOUNDNODE
#  include <osgAudio/SoundNode.h>
#else
#  include <osgAudio/SoundUpdateCB.h>
#endif
#include <osgAudio/SoundUpdateCB.h>
#include <osgAudio/SoundRoot.h>
#include <osgAudio/SoundManager.h>
#include <osgAudio/SoundState.h>
#include <osgAudio/Version.h>


osg::AnimationPath* createAnimationPath(const osg::Vec3& center,float radius,double looptime)
{
    // set up the animation path 
    osg::AnimationPath* animationPath = new osg::AnimationPath;
    animationPath->setLoopMode(osg::AnimationPath::LOOP);
    
    int numSamples = 40;
    float yaw = 0.0f;
    float yaw_delta = 2.0f*osg::PI/((float)numSamples-1.0f);
    float roll = osg::inDegrees(30.0f);
    
    double time=0.0f;
    double time_delta = looptime/(double)numSamples;
    for(int i=0;i<numSamples;++i)
    {
        osg::Vec3 position(center+osg::Vec3(sinf(yaw)*radius,cosf(yaw)*radius,0.0f));
        osg::Quat rotation(osg::Quat(roll,osg::Vec3(0.0,1.0,0.0))*osg::Quat(-(yaw+osg::inDegrees(90.0f)),osg::Vec3(0.0,0.0,1.0)));
        
        animationPath->insert(time,osg::AnimationPath::ControlPoint(position,rotation));

        yaw += yaw_delta;
        time += time_delta;
    }
    return animationPath;    
}

osg::Node* createBase(const osg::Vec3& center,float radius)
{

    int numTilesX = 10;
    int numTilesY = 10;
    
    float width = 2*radius;
    float height = 2*radius;
    
    osg::Vec3 v000(center - osg::Vec3(width*0.5f,height*0.5f,0.0f));
    osg::Vec3 dx(osg::Vec3(width/((float)numTilesX),0.0,0.0f));
    osg::Vec3 dy(osg::Vec3(0.0f,height/((float)numTilesY),0.0f));
    
    // fill in vertices for grid, note numTilesX+1 * numTilesY+1...
    osg::Vec3Array* coords = new osg::Vec3Array;
    int iy;
    for(iy=0;iy<=numTilesY;++iy)
    {
        for(int ix=0;ix<=numTilesX;++ix)
        {
            coords->push_back(v000+dx*(float)ix+dy*(float)iy);
        }
    }
    
    //Just two colours - black and white.
    osg::Vec4Array* colors = new osg::Vec4Array;
    colors->push_back(osg::Vec4(1.0f,1.0f,1.0f,1.0f)); // white
    colors->push_back(osg::Vec4(0.0f,0.0f,0.0f,1.0f)); // black
    int numColors=colors->size();
    
    
    int numIndicesPerRow=numTilesX+1;
    osg::UByteArray* coordIndices = new osg::UByteArray; // assumes we are using less than 256 points...
    osg::UByteArray* colorIndices = new osg::UByteArray;
    for(iy=0;iy<numTilesY;++iy)
    {
        for(int ix=0;ix<numTilesX;++ix)
        {
            // four vertices per quad.
            coordIndices->push_back(ix    +(iy+1)*numIndicesPerRow);
            coordIndices->push_back(ix    +iy*numIndicesPerRow);
            coordIndices->push_back((ix+1)+iy*numIndicesPerRow);
            coordIndices->push_back((ix+1)+(iy+1)*numIndicesPerRow);
            
            // one color per quad
            colorIndices->push_back((ix+iy)%numColors);
        }
    }
    

    // set up a single normal
    osg::Vec3Array* normals = new osg::Vec3Array;
    normals->push_back(osg::Vec3(0.0f,0.0f,1.0f));
    
#if OSG_VERSION_GREATER_THAN(3,2,0)
    deprecated_osg::Geometry* geom = new deprecated_osg::Geometry;
#else
    osg::Geometry* geom = new osg::Geometry;
#endif
    geom->setVertexArray(coords);
    geom->setVertexIndices(coordIndices);
    
    geom->setColorArray(colors);
    geom->setColorIndices(colorIndices);

#if OSG_VERSION_GREATER_THAN(3,2,0)
    geom->setColorBinding(deprecated_osg::Geometry::BIND_PER_PRIMITIVE);
#else
    geom->setColorBinding(osg::Geometry::BIND_PER_PRIMITIVE);
#endif

    geom->setNormalArray(normals);

#if OSG_VERSION_GREATER_THAN(3,2,0)
    geom->setNormalBinding(deprecated_osg::Geometry::BIND_OVERALL);
#else
    geom->setNormalBinding(osg::Geometry::BIND_OVERALL);
#endif

    geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS,0,coordIndices->size()));
    
    osg::Geode* geode = new osg::Geode;
    geode->addDrawable(geom);
    
    return geode;
}

osg::Node* createMovingModel(const osg::Vec3& center, float radius)
{
    float animationLength = 10.0f;

    osg::AnimationPath* animationPath = createAnimationPath(center,radius,animationLength);

    osg::Group* model = new osg::Group;

    osg::Node* glider = osgDB::readNodeFile("glider.osg");
    if (glider)
    {
        const osg::BoundingSphere& bs = glider->getBound();

        osg::BoundingSphere::value_type size = radius/bs.radius()*0.3f;
        osg::MatrixTransform* positioned = new osg::MatrixTransform;
        positioned->setDataVariance(osg::Object::STATIC);
        positioned->setMatrix(osg::Matrix::translate(-bs.center())*
                                     osg::Matrix::scale(size,size,size)*
                                     osg::Matrix::rotate(osg::inDegrees(-90.0f),0.0f,0.0f,1.0f));
    
        positioned->addChild(glider);
        
        // Create a sound node
        //osg::ref_ptr<osgAudio::SoundNode> sound_node = createSound("bee.wav");

        // Add the sound node
        //positioned->addChild(sound_node.get());

        osg::PositionAttitudeTransform* xform = new osg::PositionAttitudeTransform;    
        xform->setUpdateCallback(new osg::AnimationPathCallback(animationPath,0.0,1.0));
        xform->addChild(positioned);

        model->addChild(xform);
    }
 
    osg::Node* cessna = osgDB::readNodeFile("cessna.osg");
    if (cessna)
    {
        const osg::BoundingSphere& bs = cessna->getBound();

        osg::BoundingSphere::value_type size = radius/bs.radius()*0.3f;
        osg::MatrixTransform* positioned = new osg::MatrixTransform;
        positioned->setDataVariance(osg::Object::STATIC);
        positioned->setMatrix(osg::Matrix::translate(-bs.center())*
                                     osg::Matrix::scale(size,size,size)*
                                     osg::Matrix::rotate(osg::inDegrees(180.0f),0.0f,0.0f,1.0f));
    
        positioned->addChild(cessna);
    
        osg::MatrixTransform* xform = new osg::MatrixTransform;
        xform->setUpdateCallback(new osg::AnimationPathCallback(animationPath,0.0f,2.0));
        xform->addChild(positioned);

        model->addChild(xform);
    }
    
    return model;
}


osg::Node* createModel()
{
    osg::Vec3 center(0.0f,0.0f,0.0f);
    float radius = 100.0f;

    osg::Group* root = new osg::Group;

    root->addChild(createMovingModel(center,radius*0.8f));

    root->addChild(createBase(center-osg::Vec3(0.0f,0.0f,radius*0.5),radius));

    return root;
}


int main( int argc, char **argv )
{
    osg::notify(osg::WARN) << "\n\n" << osgAudio::getLibraryName() << " demo" << std::endl;
    osg::notify(osg::WARN) << "Version: " << osgAudio::getVersion() << "\n\n" << std::endl;

    osg::notify(osg::WARN) << "Demonstrates how to create and destroy soundsources on the fly" << std::endl;


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

        int num_hw_soundsources = 10;
        osgAudio::SoundManager::instance()->init(num_hw_soundsources);
        osgAudio::SoundManager::instance()->getEnvironment()->setDistanceModel(osgAudio::InverseDistance);
        osgAudio::SoundManager::instance()->getEnvironment()->setDopplerFactor(1);

        // load the nodes from the commandline arguments.
        osg::Node* model = createModel();
        if (!model)
        {
            return 1;
        }

        // tilt the scene so the default eye position is looking down on the model.
        osg::MatrixTransform* rootnode = new osg::MatrixTransform;
        rootnode->setMatrix(osg::Matrix::rotate(osg::inDegrees(30.0f),1.0f,0.0f,0.0f));
        rootnode->addChild(model);


        // Make use of a few sound samples
        std::vector<std::string> wave_vector;
        wave_vector.push_back("a.wav");
        wave_vector.push_back("high-e.wav");
        wave_vector.push_back("low-e.wav");


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


        // Create a transformation node onto we will attach a soundnode
        osg::ref_ptr<osg::PositionAttitudeTransform> sound_transform = new osg::PositionAttitudeTransform;
        rootnode->addChild(sound_transform.get());
        sound_transform->setPosition(osg::Vec3(0,-100,50));

        // Create a sphere so we can "see" the sound
        osg::ref_ptr<osg::Geode> geode = new osg::Geode;
        osg::TessellationHints* hints = new osg::TessellationHints;
        hints->setDetailRatio(0.5f);
        geode->addDrawable(new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(0.0f,0.0f,0.0f),1),hints));
        sound_transform->addChild(geode.get());
#ifdef USE_SOUNDNODE
        osg::ref_ptr<osgAudio::SoundNode> sound_node = new osgAudio::SoundNode;
        sound_transform->addChild(sound_node.get());
#else
        osg::ref_ptr< osgAudio::SoundUpdateCB > soundCB = new osgAudio::SoundUpdateCB;
        geode->setUpdateCallback( soundCB.get() );
#endif

        osg::Timer_t curr, start = osg::Timer::instance()->tick();
        double interval = 2; // 2 seconds interval
        unsigned int n = 0;

        // run optimization over the scene graph
        osgUtil::Optimizer optimizer;
        optimizer.optimize(rootnode);
         
        // set the scene to render
        viewer.setSceneData(rootnode);

        // create the windows and run the threads.
        viewer.realize();

        osgViewer::Viewer::Windows windows;
        viewer.getWindows(windows);
        windows[0]->setWindowRectangle( 10, 10, 1024, 768 );
        windows[0]->setWindowDecoration( true );

        while( !viewer.done() )
        {
            // wait for all cull and draw threads to complete.
            //viewer.sync();

            // update the scene by traversing it with the the update visitor which will
            // call all node update callbacks and animations.
            //viewer.update();

            // For every interval seconds, we will remove the current soundstate and create a new one,
            // with the next sample sounds
            curr = osg::Timer::instance()->tick();
            if (osg::Timer::instance()->delta_s(start, curr) > interval) 
            {
                start = curr;

                std::string file = wave_vector[n%wave_vector.size()];

                osgAudio::SoundManager::instance()->removeSoundState(file);

                n++;
                // Create a sample, load a .wav file.
                file = wave_vector[n%wave_vector.size()];
                bool add_to_cache = true;
                osg::ref_ptr<osgAudio::Sample> sample = osgAudio::SoundManager::instance()->getSample(file.c_str(), add_to_cache);
                osg::notify(osg::WARN) << "Loading sample: " << file << std::endl;

                // Create a new soundstate, give it the name of the file we loaded.
                osg::ref_ptr<osgAudio::SoundState> sound_state = new osgAudio::SoundState(file);
                sound_state->setSample(sample.get());
                sound_state->setGain(1.0f);
                sound_state->setReferenceDistance(60);
                //sound_state->setRolloffFactor(3); // FMOD backend doesn't currently support non-realistic rolloff, so this is omitted
                sound_state->setPlay(true);
                sound_state->setLooping(true);

                // Allocate a hardware soundsource to this soundstate (priority 10)
                sound_state->allocateSource(10, false);

                // Add the soundstate to the sound manager, so we can find it later on if we want to
                osgAudio::SoundManager::instance()->addSoundState(sound_state.get());

#ifdef USE_SOUNDNODE
                sound_node->setSoundState(sound_state.get());
#else
                soundCB->setSoundState( sound_state.get() );
#endif
            }

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
