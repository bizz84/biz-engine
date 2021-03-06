/*****************************************************************************
 * Filename			Weapon.h
 * 
 * License			GPLv3
 *
 * Author			Andrea Bizzotto (bizz84@gmail.com)
 *
 * Platform			LinuxX11 / OpenGL
 * 
 * Description		Weapon
 *
 *****************************************************************************/
#ifndef _BULLET_RENDERER_H_
#define _BULLET_RENDERER_H_

#include "Extensions.h"

#include <list>
using namespace std;

class Bullet;

class BulletRenderer
{
public:
	virtual ~BulletRenderer() { }
	virtual void Render(const list<Bullet *> &bullets) const = 0;
};

#endif
