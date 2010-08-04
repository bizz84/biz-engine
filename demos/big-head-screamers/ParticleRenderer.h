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
#ifndef _EXPLOSION_RENDERER_H_
#define _EXPLOSION_RENDERER_H_

#include "Extensions.h"
#include "ProgramArray.h"

#include "boost/ptr_container/ptr_list.hpp"
using namespace boost;

class ParticleEmitter;

class ParticleRenderer : private ProgramArray
{
	enum { P_PARTICLE, NUM_PROGRAMS };

public:
	ParticleRenderer();
	virtual ~ParticleRenderer() { }
	void Render(const ptr_list<ParticleEmitter> &particles) const;
};

#endif
