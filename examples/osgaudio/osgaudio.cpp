/* -*-c++-*- $Id: osgaudio.cpp */
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


#include <osgDB/FileUtils>
#include <osg/DeleteHandler>
#include <osg/Notify>
#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>
#include <osg/Geometry>
#include <osg/Geode>
#include <osgUtil/Optimizer>
#include <osgDB/Registry>
#include <osgDB/ReadFile>
#include <osgViewer/ViewerEventHandlers>
#include <osgViewer/Viewer>
#include <osgGA/TrackballManipulator>
#include <osgGA/FlightManipulator>
#include <osgGA/DriveManipulator>
#include <osgGA/KeySwitchMatrixManipulator>


#include <osgAudio/FileStream.h>
#include <osgAudio/SoundUpdateCB.h>
#include <osgAudio/SoundRoot.h>
#include <osgAudio/SoundManager.h>
#include <osgAudio/SoundState.h>
#include <osgAudio/Version.h>
#include <osgAudio/SoundNode.h>

using namespace osgAudio;


osgAudio::SoundState* createSoundState(const std::string& file);


class KeyboardHandler: public osgGA::GUIEventHandler
{
public:
    KeyboardHandler(osgAudio::SoundState *state): m_sound_state(state) {}

    bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &)
    {
        if (ea.getEventType() == osgGA::GUIEventAdapter::KEYDOWN)
        {
            if (ea.getKey() == 32) 
            {

                // Now push the soundstate to the queue so it will be played.
                // Set any of the parameters that will be activated when it 
                // is head of the queue when the sound manager processes the soundstates later on.

                // Get the current position of the listener
                float x,y,z;
                osgAudio::SoundManager::instance()->getListener()->getPosition(x,y,z);
                // Place this sound state at the position of the listener
                m_sound_state->setPosition(osg::Vec3(x,y,z));

                m_sound_state->setPlay(true); // It will play!
                m_sound_state->setPitch(1); 

                // Now push the event to the sound manager so it can play it whenever this
                // event reaches the top of the queue. A higher priority will move it to the top
                // faster!
                int priority = 10;
                osgAudio::SoundManager::instance()->pushSoundEvent(m_sound_state.get(), priority);
                return true;
            }
            else if (ea.getKey() == 'm') 
            {

                // Toggle looping ambient music

                // Try to find a soundstate named "music"
                SoundState *musicSoundState = osgAudio::SoundManager::instance()->findSoundState("music");
                if (!musicSoundState)
                {
                    // If not found, create a new one
                    musicSoundState = new SoundState("music");
                    // ALlocate a hw source so we can loop it
                    musicSoundState->allocateSource( 10 );

                    // Create a new filestream that streams samples from a ogg-file.
                    osgAudio::FileStream *musicStream = new osgAudio::FileStream("44100_1chan.ogg");

                    // Associate the stream with the sound state
                    musicSoundState->setStream( musicStream );
                    // Make it an ambient (heard everywhere) sound
                    musicSoundState->setAmbient( true );
                    // Loop the sound forever
                    musicSoundState->setLooping( true );
                    // Start playing the music!
                    //musicSoundState->setPlay( true );

                    // Add the soundstate to the sound manager so we can find it later on.
                    SoundManager::instance()->addSoundState( musicSoundState );
                }

                musicSoundState->setPlay( !musicSoundState->isPlaying() );

                return true;
            }
        }

        return false;
    }

private:
    osg::ref_ptr<osgAudio::SoundState> m_sound_state;
};



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

    deprecated_osg::Geometry* geom = new deprecated_osg::Geometry;
    geom->setVertexArray(coords);
    geom->setVertexIndices(coordIndices);

    geom->setColorArray(colors);
    geom->setColorIndices(colorIndices);
    geom->setColorBinding(deprecated_osg::Geometry::BIND_PER_PRIMITIVE);

    geom->setNormalArray(normals);
    geom->setNormalBinding(deprecated_osg::Geometry::BIND_OVERALL);

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

        // Create a sound update callback and attach a sound state to it
        osg::ref_ptr< osg::Group > group = new osg::Group;
        osg::ref_ptr< osgAudio::SoundUpdateCB > soundCB = new osgAudio::SoundUpdateCB();
        soundCB->setSoundState( createSoundState("bee.wav") );
        glider->setUpdateCallback( soundCB.get() );
        group->addChild(positioned);
    
        osg::PositionAttitudeTransform* xform = new osg::PositionAttitudeTransform;         
        xform->setUpdateCallback(new osg::AnimationPathCallback(animationPath,0.0,1.0));
        xform->addChild(group.get());
    
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

osgAudio::SoundState* createSoundState(const std::string& file)
{
    // Create a sample, load a .wav file.
    osgAudio::Sample* sample = 
        osgAudio::SoundManager::instance()->getSample(file.c_str(), false);
    // Create a named sound state.
    osgAudio::SoundState* sound_state = new osgAudio::SoundState( file );
    // Allocate a hardware soundsource to this soundstate (priority 10)
    sound_state->allocateSource(10, false);
    // Let the soundstate use the sample we just created
    sound_state->setSample(sample);

    // Set its gain (volume) to 0.9
    sound_state->setGain(0.9f);
    
    sound_state->setReferenceDistance(70);

    // Set its pitch to 1 (normal speed)
    sound_state->setPitch(1);

    // Make it play
    sound_state->setPlay(true);

    // The sound should loop over and over again
    sound_state->setLooping(true);

    // Add the soundstate to the sound manager, so we can find it later on if we want to
    osgAudio::SoundManager::instance()->addSoundState(sound_state);

    return sound_state;
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
    osg::notify(osg::NOTICE) << "\n\n" << osgAudio::getLibraryName() << " demo" << std::endl;
    osg::notify(osg::NOTICE) << "Version: " << osgAudio::getVersion() << "\n\n" << std::endl;
    osg::notify(osg::NOTICE) << "\nPress space to play a sound once...\n" << std::endl;

    try 
    {
        // use an ArgumentParser object to manage the program arguments.
        osg::ArgumentParser arguments(&argc,argv);

        // set up the usage document, in case we need to print out how to use this program.
        arguments.getApplicationUsage()->setDescription(arguments.getApplicationName()+" demonstrates the use of the osgAudio toolkit for spatial sound.");
        arguments.getApplicationUsage()->setCommandLineUsage(arguments.getApplicationName()+" [options] filename ...");
        arguments.getApplicationUsage()->addCommandLineOption("-h or --help","Display this information");

        // initialize the viewer.
        osgViewer::Viewer viewer(arguments);

        // add the window size toggle handler
        viewer.addEventHandler(new osgViewer::WindowSizeHandler);

        // add the stats handler
        viewer.addEventHandler(new osgViewer::StatsHandler);

        // add the help handler
        viewer.addEventHandler(new osgViewer::HelpHandler(arguments.getApplicationUsage()));
        
        osg::ref_ptr<osgGA::KeySwitchMatrixManipulator> keyswitchManipulator = new osgGA::KeySwitchMatrixManipulator;
        keyswitchManipulator->addMatrixManipulator( '1', "Trackball", new osgGA::TrackballManipulator() );
        viewer.setCameraManipulator( keyswitchManipulator.get() );

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

        osgAudio::SoundManager::instance()->init( 16 );
        osgAudio::SoundManager::instance()->getEnvironment()->setDistanceModel(osgAudio::InverseDistance);
        osgAudio::SoundManager::instance()->getEnvironment()->setDopplerFactor(1);
        osgAudio::SoundManager::instance()->getEnvironment()->setUnitScale(3.28);

        // Create ONE (only one, otherwise the transformation of the listener and update for SoundManager will be
        // called several times, which is not catastrophic, but unnecessary) 
        // SoundRoot that will make sure the listener is updated and
        // to keep the internal state of the SoundManager updated
        // This could also be done manually, this is just a handy way of doing it.
        osg::ref_ptr<osgAudio::SoundRoot> sound_root = new osgAudio::SoundRoot;
        
        // Specify the camera from our viewer. The view matrix from this camera
        // will be used during update to set the Listener position. Note this
        // will not work if the viewer is rendering to multiple displays; will
        // need to select a slave camera.
        sound_root->setCamera( viewer.getCamera() );
        
        // The position in the scenegraph of this node is not important.
        // Just as long as the cull traversal should be called after any changes to the SoundManager are made.
        // tilt the scene so the default eye position is looking down on the model.
        osg::MatrixTransform* rootnode = new osg::MatrixTransform;
        rootnode->setMatrix(osg::Matrix::rotate(osg::inDegrees(30.0f),1.0f,0.0f,0.0f));
        rootnode->addChild(sound_root.get());
        
        // load the nodes from the commandline arguments.
        osg::Node* model = createModel();
        if (!model)
        {
            return 1;
        }

        rootnode->addChild(model);

        // Create a sample, load a .wav file.
        osgAudio::Sample *sample = new osgAudio::Sample(
            osgDB::findDataFile( "high-e.wav" ) );
        osg::ref_ptr<osgAudio::SoundState> sound_state = new osgAudio::SoundState("high-e");
        sound_state->setSample(sample);
        sound_state->setGain(0.7f);
        sound_state->setReferenceDistance(10);
        sound_state->setPlay(false);
        sound_state->setLooping(false);

        // Add the soundstate to the sound manager, so we can find it later on if we want to
        osgAudio::SoundManager::instance()->addSoundState(sound_state.get());

        viewer.getEventHandlers().push_front(new KeyboardHandler(sound_state.get()));


        // run optimization over the scene graph after the soundnodes are added 
        // otherwise in this case the transformation added earlier would dissapear.
        osgUtil::Optimizer optimzer;
        optimzer.optimize(rootnode);

        // set the scene to render
        viewer.setSceneData(rootnode);

        // create the windows and run the threads.
        viewer.realize();

        viewer.run();
    }
    catch (std::exception& e)
    {
        osg::notify(osg::WARN) << "Caught: " << e.what() << std::endl;
    }

    // Very important to call before end of main!
    if (osg::Referenced::getDeleteHandler()) 
    {
        osg::Referenced::getDeleteHandler()->setNumFramesToRetainObjects(0);
        osg::Referenced::getDeleteHandler()->flushAll();
    }
    osgAudio::SoundManager::instance()->shutdown();
    return 0;
}
