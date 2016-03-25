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

#include <sstream>

#include <openalpp/FileStreamUpdater.h>
#include <openalpp/FileStream.h>
#include <openalpp/Sample.h>
#include <vorbis/vorbisfile.h>

#ifdef _WIN32 /* We need the following two to set stdin/stdout to binary */
#include <io.h>
#include <fcntl.h>
#endif




/************************************************************************************************************************
The following function are the vorbis callback functions.  As their names suggest, they are expected to work in exactly the
same way as normal c io functions (fread, fclose etc.).  Its up to us to return the information that the libs need to parse
the file from memory
************************************************************************************************************************/
//---------------------------------------------------------------------------------
// Function    : VorbisRead
// Purpose    : Callback for the Vorbis read function
// Info        : 
//---------------------------------------------------------------------------------
size_t VorbisRead(void *ptr            /* ptr to the data that the vorbis files need*/, 
                  size_t byteSize    /* how big a byte is*/, 
                  size_t sizeToRead /* How much we can read*/, 
                  void *datasource    /* this is a pointer to the data we passed into ov_open_callbacks (our SOggFile struct*/)
{
    FILE* file = (FILE*)datasource;

    size_t bytes_read = fread(ptr, byteSize, sizeToRead, file);

    return bytes_read; //actualSizeToRead;
}

//---------------------------------------------------------------------------------
// Function    : VorbisSeek
// Purpose    : Callback for the Vorbis seek function
// Info        : 
//---------------------------------------------------------------------------------
int VorbisSeek(void *datasource        /*this is a pointer to the data we passed into ov_open_callbacks (our SOggFile struct*/, 
               ogg_int64_t offset    /*offset from the point we wish to seek to*/, 
               int whence            /*where we want to seek to*/)
{
    FILE* file = (FILE*)datasource;

    return fseek(file, offset, whence );
}

//---------------------------------------------------------------------------------
// Function    : VorbisClose
// Purpose    : Callback for the Vorbis close function
// Info        : 
//---------------------------------------------------------------------------------
int VorbisClose(void *datasource /*this is a pointer to the data we passed into ov_open_callbacks (our SOggFile struct*/)
{
    // This file is called when we call ov_close.  If we wanted, we could free our memory here, but
    FILE* file = (FILE*)datasource;

    return fclose(file);
}

//---------------------------------------------------------------------------------
// Function    : VorbisTell
// Purpose    : Classback for the Vorbis tell function
// Info        : 
//---------------------------------------------------------------------------------
long VorbisTell(void *datasource /*this is a pointer to the data we passed into ov_open_callbacks (our SOggFile struct*/)
{
    FILE* file = (FILE*)datasource;

    return ftell(file);
}
/************************************************************************************************************************
End of Vorbis callback functions
************************************************************************************************************************/



using namespace openalpp;

FileStream::FileStream(const std::string& filename,const int buffersize)
throw (NameError,InitError,FileError) : Stream() , filename_(filename)
{
    FILE *filehandle=fopen(filename.c_str(),"rb");
    if(!filehandle)
        throw FileError("FileStream: Couldn't open file: " + filename);

    unsigned long    ulFrequency = 0;
    unsigned long    ulFormat = 0;
    unsigned long    ulBufferSize;
    unsigned long    ulChannels=0;

    oggfile_ = new OggVorbis_File;
    // Check for file type, create a FileStreamUpdater if a known type is
    // detected, otherwise throw an error.

    _vorbisCallbacks.read_func = VorbisRead;
    _vorbisCallbacks.close_func = VorbisClose;
    _vorbisCallbacks.seek_func = VorbisSeek;
    _vorbisCallbacks.tell_func = VorbisTell;



    //    if(ov_open(filehandle, oggfile_, NULL, 0)>=0) 
    if (ov_open_callbacks(filehandle, oggfile_, NULL, 0, _vorbisCallbacks)>=0)
    {
        vorbis_info *ogginfo=ov_info(oggfile_,-1);

        ulFrequency = ogginfo->rate;
        ulChannels = ogginfo->channels;

        if (ulChannels==1) {// We'll request 16 bit later...
            ulFormat=AL_FORMAT_MONO16;
            // Set BufferSize to 250ms (Frequency * 2 (16bit) divided by 4 (quarter of a second))
            ulBufferSize = ulFrequency >> 1;
            // IMPORTANT : The Buffer Size must be an exact multiple of the BlockAlignment ...
            ulBufferSize -= (ulBufferSize % 2);
            ulBufferSize = ulBufferSize*2;              
        }
        else if (ulChannels==2) {
            ulFormat=AL_FORMAT_STEREO16;
            // Set BufferSize to 250ms (Frequency * 4 (16bit stereo) divided by 4 (quarter of a second))
            ulBufferSize = ulFrequency;
            // IMPORTANT : The Buffer Size must be an exact multiple of the BlockAlignment ...
            ulBufferSize -= (ulBufferSize % 4);
            ulBufferSize = ulBufferSize*2;              
        }
        else if (ulChannels==4) {
            ulFormat = alGetEnumValue("AL_FORMAT_QUAD16");
            if (!ulFormat) 
                throw FatalError("QUAD16 format was requested but is not supported by OpenAL device");
            // Set BufferSize to 250ms (Frequency * 8 (16bit 4-channel) divided by 4 (quarter of a second))
            ulBufferSize = ulFrequency * 2;
            // IMPORTANT : The Buffer Size must be an exact multiple of the BlockAlignment ...
            ulBufferSize -= (ulBufferSize % 8);
        }
        else if (ulChannels == 6)
        {
            ulFormat = alGetEnumValue("AL_FORMAT_51CHN16");
            if (!ulFormat)
                throw FatalError("51CHN16 format was requested but is not supported by OpenAL device");

            // Set BufferSize to 250ms (Frequency * 12 (16bit 6-channel) divided by 4 (quarter of a second))
            ulBufferSize = ulFrequency * 3;
            // IMPORTANT : The Buffer Size must be an exact multiple of the BlockAlignment ...
            ulBufferSize -= (ulBufferSize % 12);
        }
        else {
            fclose(filehandle);
            std::ostringstream str;
            str << "FileStream: File " << filename << " contains " << ulChannels << " which is not recognized" << std::endl;
            throw FileError(str.str().c_str());
        }

        updater_=new FileStreamUpdater(oggfile_,
            buffer_->getName(),buffer2_->getAlBuffer(),
            ulFormat,ulFrequency,
            ulBufferSize); 

    } 
    else 
    {
        fclose(filehandle);
        throw FileError("FileStream: File of unknown type");
    }
}

FileStream::FileStream(const FileStream &stream)
: Stream((const Stream &)stream), oggfile_(stream.oggfile_)
{
}

FileStream::~FileStream() 
{
}

FileStream &FileStream::operator=(const FileStream &stream) {
    if(&stream!=this) 
    {
        Stream::operator=((const Stream &)stream);
        oggfile_ = stream.oggfile_;
    }
    return *this;
}

void FileStream::setLooping(bool loop) {
    ((FileStreamUpdater *)updater_.get())->setLooping(loop);
}

