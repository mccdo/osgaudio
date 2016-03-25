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

#include <openalpp/GroupSource.h>
#include <openalpp/windowsstuff.h>

using namespace openalpp;

GroupSource::GroupSource(float x,float y,float z) throw (NameError)
: SourceBase(x,y,z),mixed_(false) {
    alGenBuffers(1,&buffer_);
    if(alGetError()!=AL_FALSE)
        throw NameError("Error generating buffer name.");
    alSourcei(sourcename_,AL_BUFFER,buffer_);
}

void GroupSource::play() throw (InitError,FileError) {
    try {
        if(!mixed_)
            mixSources();
    } catch(InitError error) {
        throw InitError("Sources must be included before trying to play");
    }
    SourceBase::play();
}

ALfloat FilterDoppler(ALuint) {
    // This space intentionally left blank. Doppler effects don't really make
    // much sense for group sources.
    return 1.0;
}

#ifndef WIN32
#define AL_GAIN_LINEAR_LOKI                      0x20000
#endif

/**
* Ripped from AL code...
*/
ALfloat DbToLinear(ALfloat dBs) {
    static const double logtab[] = {
        0.00, 0.001, 0.002, 0.003, 0.004, 0.005, 0.01, 0.011,
        0.012, 0.013, 0.014, 0.015, 0.016, 0.02, 0.021, 0.022,
        0.023, 0.024, 0.025, 0.03, 0.031, 0.032, 0.033, 0.034,
        0.04, 0.041, 0.042, 0.043, 0.044, 0.05, 0.051, 0.052,
        0.053, 0.054, 0.06, 0.061, 0.062, 0.063, 0.064, 0.07,
        0.071, 0.072, 0.073, 0.08, 0.081, 0.082, 0.083, 0.084,
        0.09, 0.091, 0.092, 0.093, 0.094, 0.10, 0.101, 0.102,
        0.103, 0.11, 0.111, 0.112, 0.113, 0.12, 0.121, 0.122,
        0.123, 0.124, 0.13, 0.131, 0.132, 0.14, 0.141, 0.142,
        0.143, 0.15, 0.151, 0.152, 0.16, 0.161, 0.162, 0.17,
        0.171, 0.172, 0.18, 0.181, 0.19, 0.191, 0.192, 0.20,
        0.201, 0.21, 0.211, 0.22, 0.221, 0.23, 0.231, 0.24, 0.25,
        0.251, 0.26, 0.27, 0.271, 0.28, 0.29, 0.30, 0.301, 0.31,
        0.32, 0.33, 0.34, 0.35, 0.36, 0.37, 0.38, 0.39, 0.40,
        0.41, 0.43, 0.50, 0.60, 0.65, 0.70, 0.75, 0.80, 0.85,
        0.90, 0.95, 0.97, 0.99 };
        const int logmax = sizeof logtab / sizeof *logtab;
        int max = logmax;
        int min = 0;
        int mid;
        int last = -1;

        if(dBs <= 0.0)
            return 0.0;

        if(dBs >= 1.0)
            return 1.0;

        mid = (max - min) / 2;
        do {
            last = mid;

            if(logtab[mid] == dBs)
                break;

            if(logtab[mid] < dBs)
                min = mid;
            else 
                max = mid;

            mid = min + ((max - min) / 2);
        } while(last != mid);

        return ((float) mid / logmax);
}

ALfloat GroupSource::filterDistance(ALuint source,Speaker speaker) {
    ALfloat gain,maxdist,refdist,rolloff,position[3],direction[3],orientation[6];
    ALfloat iangle,oangle,ogain,dist,right[3];
    ALint relative;
#ifdef WIN32
    alGetSourcef(source,AL_GAIN,&gain);
    gain=DbToLinear(gain);
    alGetSourcef(source,AL_MAX_DISTANCE,&maxdist);
    alGetSourcef(source,AL_REFERENCE_DISTANCE,&refdist);
    alGetSourcef(source,AL_ROLLOFF_FACTOR,&rolloff);
    alGetSourcefv(source,AL_POSITION,position);
    alGetSourcefv(source,AL_DIRECTION,direction);
    alGetSourcef(source,AL_CONE_INNER_ANGLE,&iangle);
    alGetSourcef(source,AL_CONE_OUTER_ANGLE,&oangle);
    alGetSourcef(source,AL_CONE_OUTER_GAIN,&ogain);
    alGetSourcei(source,AL_SOURCE_RELATIVE,&relative);
#else
    alGetSourcefv(source,AL_GAIN_LINEAR_LOKI,&gain);
    alGetSourcefv(source,AL_MAX_DISTANCE,&maxdist);
    alGetSourcefv(source,AL_REFERENCE_DISTANCE,&refdist);
    alGetSourcefv(source,AL_ROLLOFF_FACTOR,&rolloff);
    alGetSourcefv(source,AL_POSITION,position);
    alGetSourcefv(source,AL_DIRECTION,direction);
    alGetSourcefv(source,AL_CONE_INNER_ANGLE,&iangle);
    alGetSourcefv(source,AL_CONE_OUTER_ANGLE,&oangle);
    alGetSourcefv(source,AL_CONE_OUTER_GAIN,&ogain);
    alGetSourceiv(source,AL_SOURCE_RELATIVE,&relative);
#endif

    float theta=0;
    ALfloat listener[3];
    alGetListenerfv(AL_POSITION,listener);
    alGetListenerfv(AL_ORIENTATION,orientation);
    //Normalize at and up vectors
    dist=(float)sqrt(orientation[0]*orientation[0]+
        orientation[1]*orientation[1]+
        orientation[2]*orientation[2]);
    if(dist<0.0001)
        throw FatalError("Faulty at vector in AL_ORIENTATION!");
    orientation[0]/=dist;
    orientation[1]/=dist;
    orientation[2]/=dist;
    dist=(float)sqrt(orientation[3]*orientation[3]+
        orientation[4]*orientation[4]+
        orientation[5]*orientation[5]);
    if(dist<0.0001)
        throw FatalError("Faulty up vector in AL_ORIENTATION!");
    orientation[3]/=dist;
    orientation[4]/=dist;
    orientation[5]/=dist;

    // TODO: The speaker position calculation should be moved so it's done
    // _once_ instead of once per source and speaker..

    // A vector pointing to the right from the listener can be found by
    // calculating the cross product of at and up.
    // rightx=aty*upz-upy*atz
    // righty=upx*atz-atx*upz
    // rightz=atx*upy-upx*aty
    right[0]=orientation[1]*orientation[5]-orientation[4]*orientation[2];
    right[1]=orientation[3]*orientation[2]-orientation[0]*orientation[5];
    right[2]=orientation[0]*orientation[4]-orientation[3]*orientation[1];
    // The length of the cross-product of two normalized vectors will be sqrt(2)
    float dsqrt2=(float)(5.0/sqrt(2.0));
    right[0]*=dsqrt2;
    right[1]*=dsqrt2;
    right[2]*=dsqrt2;

    if(speaker==Left) {
        listener[0]-=right[0];
        listener[1]-=right[1];
        listener[2]-=right[2];
    } else {
        listener[0]+=right[0];
        listener[1]+=right[1];
        listener[2]+=right[2];
    }

    if(relative==AL_FALSE) {  // Don't need to move it if it's relative..
        listener[0]-=position[0];
        listener[1]-=position[1];
        listener[2]-=position[2];
    }
    // Ok, listener moved, so that listener is actually a direction vector.
    // Now, we just have to normalize it, to calculate angle.
    // direction must also be normalized.
    dist=sqrt(direction[0]*direction[0]+
        direction[1]*direction[1]+
        direction[2]*direction[2]);
    if(dist>0.0001) {
        direction[0]/=dist;
        direction[1]/=dist;
        direction[2]/=dist;
    }
    dist=sqrt(listener[0]*listener[0]+
        listener[1]*listener[1]+
        listener[2]*listener[2]);
    if(dist>0.0001) {
        listener[0]/=dist;
        listener[1]/=dist;
        listener[2]/=dist;
        float dot=listener[0]*direction[0]+listener[1]*direction[1]+
            listener[2]*direction[2];
        theta=acos(dot);
    }

    int almodel;
    alGetIntegerv(AL_DISTANCE_MODEL,&almodel);
    switch(almodel) {
    case(AL_NONE):
        break;
    case(AL_INVERSE_DISTANCE_CLAMPED):
        if(dist<refdist)
            dist=refdist;
        if(dist>maxdist)
            dist=maxdist;
    case(AL_INVERSE_DISTANCE):
        gain*=refdist/(refdist+rolloff*(dist-refdist));
        if(gain>1.0)
            gain=1.0;
        if(gain<0.0)
            gain=0.0;
    }  

    if(theta<(iangle/2.0)) {
        // Should be empty, as it uses "normal" attenuation, as computed above
    } else if(theta<(oangle/2.0)) {
        gain+=0.01f;     // This is a bit weird.. Shouldn't ogain be part of this?
        gain/=2.0;
    } else {
        if(ogain<0.01)
            gain=0.01f;
        else
            gain=ogain;
    }
    return gain;
}

ALshort *GroupSource::filterReverb(Source *source,ALshort *buffer,
                                   ALsizei &size,unsigned int frequency) {
                                       if(!reverbinitiated_)
                                           return buffer;
                                       // out=in[i]+scale*in[i-delay]
                                       ALfloat delay,scale;
                                       delay=source->getReverbDelay();
                                       scale=source->getReverbScale();
                                       if(delay==0.0 || scale==0.0)
                                           return buffer;

                                       // Modify delay; 2 is for 2 channels (stereo)
                                       //               frequency*2==#samples
                                       int idelay=(int)(delay*2.0*(float)frequency*2.0);

                                       ALshort *tbuffer=(ALshort *)realloc(buffer,size+2*idelay*sizeof(ALshort));
                                       if(!tbuffer)
                                           throw MemoryError("Out of memory");
                                       buffer=tbuffer;
                                       memset(((char *)buffer)+size,0,2*idelay*sizeof(ALshort));
                                       size+=2*idelay*sizeof(ALshort);

                                       ALint amp;
                                       for(unsigned int i=idelay;i<(size/sizeof(ALshort));i++) {
                                           amp=(ALint)buffer[i]+(ALint)(scale*buffer[i-idelay]);
                                           if(amp>32767)
                                               amp=32767;
                                           else if(amp<-32768)
                                               amp=-32768;
                                           buffer[i]=(ALshort)amp;
                                       }
                                       return buffer;
}

/**
* Apply filters to source.
*/
ALshort *GroupSource::applyFilters(Source *source,ALshort *buffer,
                                   ALsizei &size,unsigned int frequency){
                                       //  Apply filters: doppler,pitch,{da,reverb,coning,minmax},listenergain
                                       //                   -       *     *   *      *      *         -

                                       ALuint sourcename=source->getAlSource();
                                       ALfloat pitch,lgain,rgain;

                                       //Pitch
#ifndef WIN32
                                       alGetSourcefv(sourcename,AL_PITCH,&pitch);
#else
                                       alGetSourcef(sourcename,AL_PITCH,&pitch);
#endif
                                       pitch*=FilterDoppler(sourcename);

                                       if(pitch>0.0) {
                                           ALsizei nsize=(ALsizei)(size/pitch);
                                           ALshort *nbuffer=(ALshort *)malloc(nsize);
                                           ALfloat index=0.0;
                                           for(unsigned int i=0;i<(nsize/(2*sizeof(ALshort)));i++) {
                                               if(index*2>(size/sizeof(ALshort)))
                                               {
                                                   free(nbuffer);
                                                   throw FatalError("index>size - we're all doomed!");
                                               }
                                               int iindex=(int)index;
                                               float fracindex=index-(float)iindex;
                                               unsigned int destl=i*2,destr=i*2+1;
                                               unsigned int sourcel=iindex*2,sourcer=iindex*2+1;

                                               // Left channel
                                               nbuffer[destl]=buffer[sourcel];
                                               if((sourcel+2)<(size/sizeof(ALshort)))
                                                   nbuffer[destl]+=(ALshort)fracindex*(buffer[sourcel+2]-
                                                   buffer[sourcel]);

                                               // Right channel
                                               nbuffer[destr]=buffer[sourcer];
                                               if((sourcer+2)<(size/sizeof(ALshort)))
                                                   nbuffer[destr]+=(ALshort)fracindex*(buffer[sourcer+2]-
                                                   buffer[sourcer]);
                                               index+=pitch;
                                           }
                                           free(buffer);
                                           buffer=nbuffer;
                                           size=nsize;
                                       }

                                       lgain=filterDistance(sourcename,Left);
                                       rgain=filterDistance(sourcename,Right);
                                       buffer=filterReverb(source,buffer,size,frequency);
                                       ALfloat min,max;                          // minmax filter
#ifndef WIN32
                                       alGetSourcefv(sourcename,AL_MIN_GAIN,&min);
                                       alGetSourcefv(sourcename,AL_MAX_GAIN,&max);
#else
                                       alGetSourcef(sourcename,AL_MIN_GAIN,&min);
                                       alGetSourcef(sourcename,AL_MAX_GAIN,&max);
#endif

                                       if(lgain<min)
                                           lgain=min;
                                       else if(lgain>max)
                                           lgain=max;
                                       if(rgain<min)
                                           rgain=min;
                                       else if(rgain>max)
                                           rgain=max;

                                       for(unsigned int i=0;i<(size/sizeof(ALshort));i++) {
                                           if(i%2)
                                               buffer[i]=(ALshort)(buffer[i]*rgain);
                                           else
                                               buffer[i]=(ALshort)(buffer[i]*lgain);
                                       }
                                       return buffer;
}

void GroupSource::mixSources(unsigned int frequency)
throw (InitError,FileError,FatalError,MemoryError,ValueError) {
    ALsizei bsize=0,size=0,loadsize=0;
    ALenum format;
    ALboolean success;
    AudioConvert converter(AL_FORMAT_STEREO16,frequency);

    if(sources_.size()<1)
        throw InitError("Sources must be included before trying to mix");

    std::cerr << ((Sample *)sources_[0]->getSound())->getFileName() << "\n";

#if OPENAL_VERSION < 2006
    ALshort *loaddata=NULL,*data=NULL,*bdata=NULL;
    ALsizei bits;
    ALsizei freq;
    success=
        alutLoadWAV(((Sample *)sources_[0]->getSound())->getFileName().c_str(),
        (ALvoid **)&loaddata,&format,&loadsize,&bits,&freq);
#else
#if OPENAL_VERSION < 2007
    ALshort *loaddata=NULL,*data=NULL,*bdata=NULL;
    ALsizei bits;
    ALsizei freq;
    ALboolean loop;
    alutLoadWAVFile((ALbyte*)((Sample *)sources_[0]->getSound())->getFileName().c_str(),
        &format,(ALvoid **)&loaddata,&loadsize,&freq,&loop);
    if (loaddata)
        success=AL_TRUE;
    else 
        success=AL_FALSE;
#else
    ALvoid *loaddata=NULL;
    ALshort* data=NULL,*bdata=NULL;
    ALfloat freq;
    loaddata = alutLoadMemoryFromFile(((Sample *)sources_[0]->getSound())->getFileName().c_str(),
        &format,
        &loadsize,
        &freq);
    success = AL_FALSE;
    if (loaddata)
        success = AL_TRUE;
#endif
#endif
    if(success==AL_FALSE || !loaddata)
        throw FileError("Error opening file for mixing");

    bsize=loadsize;
    unsigned int tsize=(unsigned int)bsize;
    bdata=(ALshort *)converter.apply((void*)loaddata,format,
        (unsigned int)freq,tsize);
    bsize=tsize;

    if(!bdata)
        throw FatalError("Error converting data to internal format!");

    free(loaddata);

    bdata=applyFilters(sources_[0].get(),bdata,bsize,frequency);

    for(unsigned int s=1;s<sources_.size();s++) {
#if OPENAL_VERSION < 2006
        success=
            alutLoadWAV(((Sample *)sources_[s]->getSound())->getFileName().c_str(),
            (ALvoid **)&loaddata,&format,&loadsize,&bits,&freq);
#else
#if OPENAL_VERSION < 2007
        alutLoadWAVFile((ALbyte*)((Sample *)sources_[s]->getSound())->getFileName().c_str(),
            &format,(ALvoid **)&loaddata,&loadsize,&freq,&loop);
        if (loaddata)
            success=AL_TRUE;
        else
            success=AL_FALSE;
#else
        loaddata = alutLoadMemoryFromFile(((Sample *)sources_[s]->getSound())->getFileName().c_str(),
            &format,
            &loadsize,
            &freq);
        success = AL_FALSE;
        if (loaddata)
            success = AL_TRUE;
#endif
#endif
        if(success==AL_FALSE || !loaddata)
            throw FileError("Error opening file for mixing");

        size=loadsize;
        unsigned int tsize2=(unsigned int)size;
        data=(ALshort *)converter.apply(loaddata,format,
            (unsigned int)freq,tsize2);
        size=tsize2;

        if(!data)
            throw FatalError("Error converting data to internal format!");

        free(loaddata);

        data=applyFilters(sources_[s].get(),data,size,frequency);

        if(size>bsize) {
            loaddata=bdata;
            bdata=data;
            data=(ALshort*)(loaddata);
            loadsize=bsize;
            bsize=size;
            size=loadsize;
        } 
        ALint amp;
        for(ALsizei i=0;i<(size/2);i++) {
            amp=bdata[i]+data[i];
            if(amp>32767)
                amp=32767;
            else if(amp<-32768)
                amp=-32768;
            bdata[i]=(ALshort)amp;
        }

        free(data);
    }

    alBufferData(buffer_,AL_FORMAT_STEREO16,bdata,bsize,frequency);
    ALenum error=alGetError();
    if(error==AL_OUT_OF_MEMORY)
        throw MemoryError("Error buffering data");
    else if(error==AL_INVALID_VALUE)
        throw ValueError("Audio conversion failed");
    else if(error!=AL_NO_ERROR)
        throw FatalError((const char *)alGetString(error));

    mixed_=true;  
    free(bdata);
}

ALuint GroupSource::includeSource(Source *source) throw (ValueError) {
    if(source->isStreaming())
        throw ValueError("Can't include streaming sources in group.");
    sources_.push_back(source);
    return source->getAlSource();
}

void GroupSource::excludeSource(const Source &source) throw (NameError) {
    ALuint sourcename=source.getAlSource();
    excludeSource(sourcename);
}

void GroupSource::excludeSource(ALuint sourcename) throw (NameError) {
    for(unsigned int i=0;i<sources_.size();i++) {
        if(sourcename==sources_[i]->getAlSource()) {
            sources_[i]=sources_[sources_.size()-1];
            sources_.pop_back();
            return;
        }
    }
    throw NameError("Trying to exclude source that has not been included.");
}

GroupSource::GroupSource(const GroupSource &groupsource) : SourceBase(groupsource) {
    sources_=groupsource.sources_;
    mixed_=false;
    alGenBuffers(1,&buffer_);
    if(alGetError()!=AL_FALSE)
        throw NameError("Error generating buffer name.");
    alSourcei(sourcename_,AL_BUFFER,buffer_);
}

GroupSource::~GroupSource() {
    alDeleteBuffers(1,&buffer_);
}

GroupSource &GroupSource::operator=(const GroupSource &groupsource) {
    if(this!=&groupsource) {
        sources_.clear();
        sources_=groupsource.sources_;
        mixed_=false;
    }
    return *this;
}


