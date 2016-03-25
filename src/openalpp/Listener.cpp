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
#include <openalpp/Listener.h>

using namespace openalpp;

Listener *Listener::selectedlistener_=NULL;

/**
* Internal function for initialisation. 
*/
void Listener::init(float x,float y,float z,
                    float vx,float vy,float vz,
                    float directionx, float directiony, float directionz,
                    float upx, float upy, float upz)
{
    setPosition(x,y,z);
    setVelocity(vx,vy,vz);
    setOrientation(directionx,directiony,directionz,
        upx,upy,upz);
    if(!selectedlistener_)
        select();
}

Listener::Listener() : PositionedObject() {
    init(0,0,0, 0,0,0, 0,0,-1, 0,1,0);
}

Listener::Listener(const Listener &listener) : PositionedObject() {
    listener.getPosition(position_[0],position_[1],position_[2]);
    listener.getVelocity(velocity_[0],velocity_[1],velocity_[2]);
    listener.getOrientation(orientation_[0],orientation_[1],orientation_[2],
        orientation_[3],orientation_[4],orientation_[5]);
}

Listener::Listener(float x,float y,float z,
                   float directionx, float directiony, float directionz,
                   float upx, float upy, float upz) : PositionedObject() {

                       init(x,y,z, 0,0,0, directionx,directiony,directionz, upx,upy,upz);

}

Listener::Listener(float x, float y, float z) : PositionedObject() {
    init(x,y,z, 0,0,0, 0,0,-1, 0,1,0);
}

Listener::~Listener() {
    if(selectedlistener_==this)
        selectedlistener_=NULL;
}

void Listener::select() {
    alListenerfv(AL_POSITION,position_);
    alListenerfv(AL_VELOCITY,velocity_);
    alListenerfv(AL_ORIENTATION,orientation_);
    selectedlistener_=this;
}

bool Listener::isSelected() {
    return (selectedlistener_==this);
}

void Listener::setOrientation(float directionx,float directiony,
                              float directionz,
                              float upx, float upy, float upz) {
                                  orientation_[0]=directionx;
                                  orientation_[1]=directiony;
                                  orientation_[2]=directionz;
                                  orientation_[3]=upx;
                                  orientation_[4]=upy;
                                  orientation_[5]=upz;
                                  if(isSelected())
                                      alListenerfv(AL_ORIENTATION,orientation_);
}

void Listener::getOrientation(float &directionx,float &directiony,
                              float &directionz,
                              float &upx, float &upy, float &upz) const {
                                  directionx=orientation_[0];
                                  directiony=orientation_[1];
                                  directionz=orientation_[2];
                                  upx=orientation_[3];
                                  upy=orientation_[4];
                                  upz=orientation_[5];
}

Listener &Listener::operator=(const Listener &listener) {
    if(this!=&listener) {
        listener.getPosition(position_[0],position_[1],position_[2]);
        listener.getVelocity(velocity_[0],velocity_[1],velocity_[2]);
        listener.getOrientation(orientation_[0],orientation_[1],orientation_[2],
            orientation_[3],orientation_[4],orientation_[5]);
    }
    return *this;
}

void Listener::setPosition(float x, float y, float z) {
    position_[0]=x;
    position_[1]=y;
    position_[2]=z;
    if(isSelected())
        alListenerfv(AL_POSITION,position_);
}

void Listener::getPosition(float &x, float &y, float &z) const {
    x=position_[0];
    y=position_[1];
    z=position_[2];
}

void Listener::setVelocity(float vx, float vy, float vz) {
    velocity_[0]=vx;
    velocity_[1]=vy;
    velocity_[2]=vz;
    if(isSelected())
        alListenerfv(AL_VELOCITY,velocity_);
}

void Listener::getVelocity(float &vx, float &vy, float &vz) const {
    vx=velocity_[0];
    vy=velocity_[1];
    vz=velocity_[2];
}

