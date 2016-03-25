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

#include <osgAudio/Listener.h>
#include <openalpp/Listener.h>

using namespace osgAudio;

Listener::Listener() {
    _openalppListener = new openalpp::Listener ();
} // Listener::Listener

Listener::Listener(const Listener &listener) {
    _openalppListener = new openalpp::Listener (*listener.getInternalListener());
} // Listener::Listener

Listener::Listener(float x,float y,float z,
                   float directionx, float directiony, float directionz,
                   float upx, float upy, float upz) {

    _openalppListener = new openalpp::Listener (x, y, z,
                   directionx, directiony, directionz,
                   upx, upy, upz);

} // Listener::Listener

Listener::Listener(float x, float y, float z) {
    _openalppListener = new openalpp::Listener (x, y, z);
} // Listener::Listener

Listener::~Listener() {
// nothing to do, ref_ptr auto-releases _openalppListener
} // Listener::~Listener

void Listener::select() {
    _openalppListener->select();
}

bool Listener::isSelected() {
    return (_openalppListener->isSelected());
}

void Listener::setOrientation(float directionx,float directiony,
                              float directionz,
                              float upx, float upy, float upz) {
    _openalppListener->setOrientation(directionx, directiony, directionz, upx, upy, upz);
}

void Listener::getOrientation(float &directionx,float &directiony,
                              float &directionz,
                              float &upx, float &upy, float &upz) const {
    _openalppListener->getOrientation(directionx, directiony, directionz, upx, upy, upz);
}

Listener &Listener::operator=(const Listener &listener) {
    if(this!=&listener) {
    _openalppListener = new openalpp::Listener (*listener.getInternalListener());
    }
    return *this;
}

void Listener::setPosition(float x, float y, float z) {
    _openalppListener->setPosition(x, y, z);
}

void Listener::getPosition(float &x, float &y, float &z) const {
    _openalppListener->getPosition(x, y, z);
}

void Listener::setVelocity(float vx, float vy, float vz) {
    _openalppListener->setVelocity(vx, vy, vz);
}

void Listener::getVelocity(float &vx, float &vy, float &vz) const {
    _openalppListener->getVelocity(vx, vy, vz);
}
