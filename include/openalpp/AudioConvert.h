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

#ifndef OPENALPP_AUDIOCONVERT_H
#define OPENALPP_AUDIOCONVERT_H 1

#include <al.h>
#include <stdlib.h>
#include <string.h>

#include <openalpp/Export.h>
#include <openalpp/Error.h>

namespace openalpp {

    /**
    * Class for converting audio.
    */
    class OPENALPP_API AudioConvert {
        unsigned short channels_,bits_;
        unsigned int frequency_;
        ALenum format_;
    public:
        /**
        * Constructor.
        * @param format is the (OpenAL) format that data will be converted to.
        * @param frequency is the frequency the data will be converted to.
        */
        AudioConvert(ALenum format,unsigned int frequency);

        /**
        * Apply the conversion to data.
        * @param data is the data to convert.
        * @param format is the (OpenAL) format of the data.
        * @param frequency is the frequency of the data.
        * @param size is the size of the data. It will be updated to the new size.
        */
        void *apply(void *data,ALenum format,unsigned int frequency,unsigned int &size);
    };

    typedef struct _acAudioCVT {
        int needed;                     /* Set to 1 if conversion possible */
        ALushort src_format;            /* Source audio format */
        ALushort dst_format;            /* Target audio format */
        double rate_incr;               /* Rate conversion increment */
        void   *buf;                    /* Buffer to hold entire audio data */
        int    len;                     /* Length of original audio buffer */
        int    len_cvt;                 /* Length of converted audio buffer */
        int    len_mult;                /* buffer must be len*len_mult big */
        double len_ratio;       /* Given len, final size is len*len_ratio */
        void (*filters[10])(struct _acAudioCVT *cvt, ALushort format);
        int filter_index;               /* Current audio conversion function */
    } acAudioCVT;

/* Audio format flags (defaults to LSB byte order) */
#define AUDIO_U8        0x0008  /* Unsigned 8-bit samples */
#define AUDIO_S8        0x8008  /* Signed 8-bit samples */
#define AUDIO_U16LSB    0x0010  /* Unsigned 16-bit samples */
#define AUDIO_S16LSB    0x8010  /* Signed 16-bit samples */
#define AUDIO_U16MSB    0x1010  /* As above, but big-endian byte order */
#define AUDIO_S16MSB    0x9010  /* As above, but big-endian byte order */

/* Native audio byte ordering */
#ifndef WORDS_BIGENDIAN
#define AUDIO_U16       AUDIO_U16LSB
#define AUDIO_S16       AUDIO_S16LSB
#define swap16le(x) (x)
#define swap32le(x) (x)
#define swap16be(x) swap16(x)
#define swap32be(x) swap32(x)
#else
#define AUDIO_U16       AUDIO_U16MSB
#define AUDIO_S16       AUDIO_S16MSB
#define swap16le(x) swap16(x)
#define swap32le(x) swap32(x)
#define swap16be(x) (x)
#define swap32be(x) (x)
#endif

#define _al_ALCHANNELS(fmt) ((fmt==AL_FORMAT_MONO16||fmt==AL_FORMAT_MONO8)?1:2)

#define DATA            0x61746164              /* "data" */
#define FACT            0x74636166              /* "fact" */
#define LIST            0x5453494c              /* "LIST" */
#define RIFF            0x46464952
#define WAVE            0x45564157
#define FMT             0x20746D66

#define AL_FORMAT_IMA_ADPCM_MONO16_EXT            0x10000
#define AL_FORMAT_IMA_ADPCM_STEREO16_EXT          0x10001
#define AL_FORMAT_WAVE_EXT                        0x10002

#define NELEMS(x) ((sizeof x) / (sizeof *x))

#define PCM_CODE        0x0001
#define MS_ADPCM_CODE   0x0002
#define IMA_ADPCM_CODE  0x0011

#define MS_ADPCM_max ((1<<(16-1))-1)
#define MS_ADPCM_min -(1<<(16-1))

    typedef struct Chunk {
        ALuint magic;
        ALuint length;
        void *data;
    } Chunk;

    struct MS_ADPCM_decodestate_FULL {
        ALubyte hPredictor;
        ALushort iDelta;
        ALshort iSamp1;
        ALshort iSamp2;
    };

    typedef struct WaveFMT {
        ALushort encoding;
        ALushort channels;               /* 1 = mono, 2 = stereo */
        ALuint frequency;              /* One of 11025, 22050, or 44100 Hz */
        ALuint byterate;               /* Average bytes per second */
        ALushort blockalign;             /* Bytes per sample block */
        ALushort bitspersample;
    } alWaveFMT_LOKI;

    typedef struct IMA_ADPCM_decodestate_s {
        ALint valprev;  /* Previous output value */
        ALbyte index;           /* Index into stepsize table */
    } alIMAADPCM_decodestate_LOKI;

    typedef struct IMA_ADPCM_decoder {
        alWaveFMT_LOKI wavefmt;
        ALushort wSamplesPerBlock;
        alIMAADPCM_decodestate_LOKI state[2];
    } alIMAADPCM_state_LOKI;
    /*
    void *AudioConvert(ALvoid *data,
    ALenum f_format, ALuint f_size, ALuint f_freq,
    ALenum t_format, ALuint t_freq, ALuint *retsize);
    */
}

#endif // OPENALPP_AUDIOCONVERT_H
