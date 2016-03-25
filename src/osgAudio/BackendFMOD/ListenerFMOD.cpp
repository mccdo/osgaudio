/* -*-c++-*- */
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

#include <osgAudio/Listener.h>
#include <osgAudio/AudioEnvironment.h>

using namespace osgAudio;

osg::observer_ptr<osgAudio::Listener> osgAudio::Listener::_selectedlistener;

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
	if(!isSelected())
		select();
}

Listener::Listener() 
{
	init(0,0,0, 0,0,0, 0,0,-1, 0,1,0);
}

Listener::Listener(const Listener &listener) 
{
	float transfer[6];
	listener.getPosition(transfer[0],transfer[1],transfer[2]);
	setPosition(transfer[0],transfer[1],transfer[2]);
	listener.getVelocity(transfer[0],transfer[1],transfer[2]);
	setVelocity(transfer[0],transfer[1],transfer[2]);
	listener.getOrientation(transfer[0],transfer[1],transfer[2],
		transfer[3],transfer[4],transfer[5]);
	setOrientation(transfer[0],transfer[1],transfer[2],
		transfer[3],transfer[4],transfer[5]);
}

Listener::Listener(float x,float y,float z,
				   float directionx, float directiony, float directionz,
				   float upx, float upy, float upz) 
{
    init(x,y,z, 0,0,0, directionx,directiony,directionz, upx,upy,upz);
}

Listener::Listener(float x, float y, float z) 
{
	init(x,y,z, 0,0,0, 0,0,-1, 0,1,0);
}

Listener::~Listener() 
{
}

void Listener::select() 
{
	_selectedlistener = this; // we are now the selected Listener
	osgAudio::AudioEnvironment::instance()->getSystem()->
        set3DListenerAttributes(0, &_pos, &_vel, &_forward, &_up);
}

bool Listener::isSelected() 
{
	return (_selectedlistener.valid() && _selectedlistener.get() == this);
}

void Listener::setOrientation(float directionx,float directiony,
							  float directionz,
							  float upx, float upy, float upz) 
{
    _forward.x=directionx;
    _forward.y=directiony;
    _forward.z=directionz;
    _up.x=upx;
    _up.y=upy;
    _up.z=upz;
    osgAudio::AudioEnvironment::instance()->getSystem()->
        set3DListenerAttributes(0, &_pos, &_vel, &_forward, &_up);
}

void Listener::getOrientation(float &directionx,float &directiony,
							  float &directionz,
							  float &upx, float &upy, float &upz) const 
{
    directionx=_forward.x;
    directiony=_forward.y;
    directionz=_forward.z;
    upx=_up.x;
    upy=_up.y;
    upz=_up.z;
}

Listener &Listener::operator=(const Listener &listener) 
{
	if(this!=&listener) 
    {
		float transfer[6];
		listener.getPosition(transfer[0],transfer[1],transfer[2]);
		setPosition(transfer[0],transfer[1],transfer[2]);
		listener.getVelocity(transfer[0],transfer[1],transfer[2]);
		setVelocity(transfer[0],transfer[1],transfer[2]);
		listener.getOrientation(transfer[0],transfer[1],transfer[2],
			transfer[3],transfer[4],transfer[5]);
		setOrientation(transfer[0],transfer[1],transfer[2],
			transfer[3],transfer[4],transfer[5]);
	}
	return *this;
}

void Listener::setPosition(float x, float y, float z) 
{
	_pos.x=x;
	_pos.y=y;
	_pos.z=z;
	osgAudio::AudioEnvironment::instance()->getSystem()->
        set3DListenerAttributes(0, &_pos, &_vel, &_forward, &_up);
}

void Listener::getPosition(float &x, float &y, float &z) const 
{
	x=_pos.x;
	y=_pos.y;
	z=_pos.z;
}

void Listener::setVelocity(float vx, float vy, float vz)
{
	_vel.x=vx;
	_vel.y=vy;
	_vel.z=vz;
	osgAudio::AudioEnvironment::instance()->getSystem()->
        set3DListenerAttributes(0, &_pos, &_vel, &_forward, &_up);
}

void Listener::getVelocity(float &vx, float &vy, float &vz) const 
{
	vx=_vel.x;
	vy=_vel.y;
	vz=_vel.z;
}
