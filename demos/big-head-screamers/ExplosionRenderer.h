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
#ifndef _WEAPON_RENDERER_H_
#define _WEAPON_RENDERER_H_

#include "Extensions.h"
#include "VBO.h"
#include "ProgramArray.h"

#include <list>
using namespace std;

class Bullet;
class BulletExplosion;

class WeaponRenderer : private ProgramArray
{
	enum {
		P_LOOKUP_COLOR,
		P_PARTICLE,
		NUM_PROGRAMS 
	};

	IndexedVBO *pTetraVBO;
public:
	WeaponRenderer();
	~WeaponRenderer();
	void Render(const list<Bullet *> &bullets) const;
	void Render(const list<BulletExplosion *> &explosions) const;
};

#endif
