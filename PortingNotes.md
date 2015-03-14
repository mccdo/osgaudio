# Porting Notes #

If you are porting your app from osgAL to osgAudio, you should read these porting notes and change your application to work with osgAudio.


## Positioning the Listener ##

In order to position the Listener correctly, osgAudio requires your application to attach a Camera to the SoundRoot node using SoundRoot::setCamera(). Here's the background information...

In osgAudio, SoundRoot uses an update callback to do the tasks formerly done in cull,
such as process queued sound events and update the Listener position. In osgAL, SoundRoot obtained the Camera view matrix from the CullVisitor, and
used it to update the Listener position. This behavior was ill-defined in the
case of multiple views.

osgAudio requires an application to specify a Camera to the
SoundRoot using the setCamera() method. Typically, an app will do this
immediately after creating the SoundRoot, or as soon as the Camera is available
(such as after viewer.realize()), and usually before the first frame. SoundRoot
will then use this Camera's view matrix to determine and update the Listener
position during the update traversal.

If your app has a single view, you would typically specify that view's Camera.

```
//Something like this with an osgViewer based app
sound_root->setCamera( viewer.getCamera() );
```

If your app has multiple views, you will need to decide which view you'd like to
use for the Listener position, and specify that view's Camera.

If you don't specify a Camera, SoundRoot will not specify the Listener's position.
This allows the application to specify the Listener position
explicitly, which was not possible in osgAL. To do this, don't bother setting a Camera in the [SoundRoot](http://code.google.com/p/osgaudio/source/browse/trunk/include/osgAudio/SoundRoot.h). Go ahead
and leave this NULL. Then, after the update traversal, and before cull/draw,
explicitly set the Listener position as follows:

```
osgAudio::SoundManager::instance()->setListenerMatrix( m );
```

The osg::Matrix 'm' is a view matrix representing the Listener's position and
orientation.

Note this change also affects loading a saved osgAL file containing a SoundRoot. Your application will now need to search the loaded scene graph and attach a Camera to the SoundRoot node.

This change was made in response to [issue 3](http://code.google.com/p/osgaudio/issues/detail?id=3).


## Using SoundUpdateCB ##

The osgAL SoundNode class is not thread safe. It still exists in osgAudio for 100% bug compatibility, but applications are strongly encouraged to replace SoundNode with the new osgAudio SoundUpdateCB, an update callback that can be attached to any node to play a sound at that node's location.

Derived from NodeCallback, SoundUpdateCB performs the same tasks as SoundNode, but does this work once in the update traversal, instead of possibly multiple times in the cull traversal. See example\_osgaudio\_multiple and example\_osgaudio, which have replaced SoundNode with SoundUpdateCB.

## OccluderCallback ##

The SoundUpdateCB change forced a change to the interface of OccluderCallback. osgAudio removes OccluderCallback's dependency on SoundNode, and instead OccluderCallback requires only a SoundState. This allows both SoundNode and SoundUpdateCB to work with OccluderCallback.

## Displaying Initialization Messages ##

A new method has been added to display initialization messages from FMOD and from OpenAL.
```
void SoundManager::init( unsigned int num_soundsources, bool displayInitMsgs )

void SoundManager::init(unsigned int num_soundsources, float sound_velocity, bool displayInitMsgs )
```