# osgAudio #

**osgAudio** is an [OpenSceneGraph](http://www.openscenegraph.org) nodekit that wraps OpenAL and FMOD to enable sound toolkits in OSG. The **osgAudio** library is the result of collaborative work between [Ames Lab](http://www.ameslab.gov/), [ARDEC](http://www.pica.army.mil/PicatinnyPublic/index.asp), Chris Hanson ([Alpha Pixel, LLC](http://www.alphapixel.com/)), and Paul Martz ([Skew Matrix Software LLC](http://www.skew-matrix.com)). It's used as part of the [VE-Suite](http://www.ve-suite.org/) project. **osgAudio** is open source and available under the GNU LGPL v2.1 software license.

# Features #

**osgAudio** supports a compile-time selectable audio back end, which determines support for speaker configurations and specific audio file formats. Common speaker configurations (stereo, 5.1 surround) and file formats (.wav) are supported by both the OpenAL and FMOD back ends (within the limits of OpenAL and FMOD).

## Libraries ##

The **osgAudio** library Includes the following features:
  * Compile-time switchable audio back ends (OpenAL and FMOD currently supported).
  * Streaming audio.
  * Positionable listener and sound sources.
  * Ambient sounds and sound occlusion.
  * Attenuation and Doppler shifting.

## Plugins ##

**osgdb\_osgAudio** Dot OSG file format support for the nodes and objects in the osgAudio library.

## Examples ##

The project contains a small collection of example programs to demonstrate use of many of **osgAudio's** features.

## Tests ##

The project contains several test programs to ensure correct functionality. Some test programs are specific to the OpenAL/OpenAL++ backend, but equivalent non-backend-specific tests also are included.

# Building #

See the [Installation](Installation.md) page.

# Support #

For support requests beyond issuing bugs through google code please contact  [Skew Matrix Software, LLC](http://www.skew-matrix.com).

# Contribute #

All contributions are welcome and will be considered for inclusion in the project. Please contribute any enhancements or bug fixes by [opening an issue](http://code.google.com/p/osgaudio/issues/list) and use the **Contribution from user** issue template. Create and attach a compressed patch file containing your changes.