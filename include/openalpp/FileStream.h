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

#ifndef OPENALPP_FILESTREAM_H
#define OPENALPP_FILESTREAM_H 1

#include <openalpp/Stream.h>
#include <openalpp/Export.h>

#if 0        // #ifdef _DEBUG
// for memory leak reporting
#include <crtdbg.h>
#define DEBUG_NEW new(_NORMAL_BLOCK ,__FILE__, __LINE__)
#else
#define DEBUG_NEW new
#endif

#include <vorbis/vorbisfile.h>


struct OggVorbis_File;

namespace openalpp {

    /**
    * Class for streaming audio data from a file. Presently it supports Ogg
    * Vorbis files (http://www.vorbis.com/).
    */
    class OPENALPP_API  FileStream : public Stream {
    public:
        /**
        * Constructor. A FileError will be thrown if the file can't be opened or
        * isn't recognized.
        * @param filename is the name of the file to try to open.
        * @param buffersize is an optional parameter specifying how large the
        * buffer should be (in samples per second).
        */
        FileStream(const std::string& filename,const int buffersize=4096) 
            throw (NameError,InitError,FileError);

        /**
        * Copy constructor.
        */
        FileStream(const FileStream &stream);


        FileStream &operator=(const FileStream &stream);

        /**
        * Turn on/off looping.
        * @param loop is true if the stream should loop, false otherwise.
        */
        void setLooping(bool loop = true);

        std::string getFileName() const { return filename_; }

    protected:
        /**
        * Destructor.
        */
        virtual ~FileStream();

        ov_callbacks    _vorbisCallbacks;    // callbacks used to read the file from memory

        std::string filename_;


        OggVorbis_File* oggfile_;

    };


}

#endif // OPENALPP_FILESTREAM_H
