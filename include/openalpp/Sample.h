/* -*-c++-*- */
/**
 * osgAudio - OpenSceneGraph Audio Library
 * (C) Copyright 2009-2012 by Kenneth Mark Bryden
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

#ifndef OPENALPP_SAMPLE_H
#define OPENALPP_SAMPLE_H 1

#include <openalpp/windowsstuff.h>
#include <openalpp/SoundData.h>
#include <openalpp/Error.h>

#include <string>

namespace openalpp {

    /**
    * Class for loading sampled files.
    */
    class OPENALPP_API Sample : public SoundData {
    public:
        /**
        * Constructor.
        * @param filename is name of file to load.
        */
        Sample(const std::string& filename ) throw (FileError);

        /**
        * Copy constructor.
        */
        Sample(const Sample &sample);

        /**
        * Constructor.
        * @param format to use to create sample from data.
        * @param data use to create sample.
        * @param size of data.
        * @param freq of data.
        */
        Sample(ALenum format,ALvoid* data,ALsizei size,ALsizei freq) throw (FileError);


        /**
        * Get file name of loaded file.
        * @return file name.
        */
        std::string getFileName() const;

        /**
        * Assignment operator.
        */
        Sample &operator=(const Sample &sample);

    protected:
        /**
        * Destructor
        */
        virtual ~Sample();

    private:
        /**
        * File name.
        */
        std::string filename_;

    };

    /**
    * Check how large a sample is in the given format.
    */
    unsigned int sampleSize(SampleFormat format);
    unsigned int sampleSize(ALenum format);
}

#endif /* OPENALPP_SAMPLE_H */
