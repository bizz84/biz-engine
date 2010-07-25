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
#ifndef _PARTICLE_EMITTER_H_
#define _PARTICLE_EMITTER_H_

#include "Extensions.h"
#include "Matrix.h"
#include "CameraController.h"

#include <vector>
#include <list>

using namespace std;

class ParticleEmitter
{
protected:
	class Particle
	{
	public:
		Point4 pos;
		Vector3 vel;
		int hit;
	};
	Particle *particle;
	unsigned int numParticles;
	bool expired;

	virtual bool ParticleUpdate(Particle *p, const float dt) = 0;

public:
	ParticleEmitter(const int n);
	virtual ~ParticleEmitter();

	virtual bool Update(const float dt);

	bool Expired() const { return expired; }

	virtual void Render() { }
};

class BloodDropEmitter : public ParticleEmitter
{
protected:
	class BloodParticle : public Particle
	{
	public:
	};

	virtual bool ParticleUpdate(Particle *p, const float dt);

public:
	BloodDropEmitter(const Point3 &pos, const int n);

	virtual void Render();
};

#endif
