/*****************************************************************************
 * Filename			Weapon.h
 * 
 * License			LGPL
 *
 * Author			Andrea Bizzotto (bizz84@gmail.com)
 *
 * Platform			LinuxX11 / OpenGL
 * 
 * Description		Weapon
 *
 *****************************************************************************/
#ifndef _LASER_RENDERER_H_
#define _LASER_RENDERER_H_

#include "Extensions.h"
#include "ProgramArray.h"
#include "BulletRenderer.h"

#include <list>
using namespace std;

class Bullet;

class LaserRenderer : public BulletRenderer, private ProgramArray
{
	enum { P_LASER, NUM_PROGRAMS };

public:
	LaserRenderer();
	~LaserRenderer() { }
	virtual void Render(const list<Bullet *> &bullets) const;
};

#endif
