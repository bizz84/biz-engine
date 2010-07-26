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
#ifndef _GRENADE_RENDERER_H_
#define _GRENADE_RENDERER_H_

#include "Extensions.h"
#include "ProgramArray.h"
#include "BulletRenderer.h"

#include <list>
using namespace std;

class Bullet;
class Mesh;

class GrenadeRenderer : public BulletRenderer, private ProgramArray
{
	enum { P_GRENADE, NUM_PROGRAMS };

	Mesh *pMesh[2];
public:
	GrenadeRenderer();
	~GrenadeRenderer();
	virtual void Render(const list<Bullet *> &bullets) const;
	virtual void Render(const Bullet *bullet) const;
};

#endif
