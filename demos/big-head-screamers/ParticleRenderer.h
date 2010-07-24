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
#ifndef _EXPLOSION_RENDERER_H_
#define _EXPLOSION_RENDERER_H_

#include "Extensions.h"
#include "ProgramArray.h"

#include <list>
using namespace std;

class ParticleEmitter;

class ExplosionRenderer : private ProgramArray
{
	enum { P_PARTICLE, NUM_PROGRAMS };

public:
	ExplosionRenderer();
	virtual ~ExplosionRenderer() { }
	void Render(const list<ParticleEmitter *> &explosions) const;
};

#endif
