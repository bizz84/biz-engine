/*****************************************************************************
 * Filename			Weapon.cpp
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

#include "ParticleEmitter.h"


#include "Extensions.h"
#include "Misc.h"
#include "Settings.h"

ParticleEmitter::ParticleEmitter(const int n)
	: numParticles(n), expired(false)
{

}
ParticleEmitter::~ParticleEmitter()
{
	delete [] particle;
}


bool ParticleEmitter::Update(const float dt)
{
	expired = true;
	for (unsigned int i = 0; i < numParticles; i++)
	{
		if (ParticleUpdate(&particle[i], dt))
			expired = false;
	}
	return !expired;
}


BloodDropEmitter::BloodDropEmitter(const Point3 &pos, const int n) : ParticleEmitter(n)
{
	particle = new BloodParticle[n];
	BloodParticle *q = (BloodParticle *)particle;
	for (unsigned int i = 0; i < numParticles; i++)
	{
		q->pos = Point4(pos[0], pos[1], pos[2], 1.0f);
		q->vel = RandSphere() * Settings::Instance().ParticleSpeed;
		q->hit = 0;
		q++;
	}
}

bool BloodDropEmitter::ParticleUpdate(Particle *p, const float dt)
{
	BloodParticle *q = (BloodParticle *)p;
	float f = Settings::Instance().ParticleGravity * dt;
	q->vel[1] -= f;

	Vector3 dir = q->vel * dt;
	q->pos += Vector4(dir[0], dir[1], dir[2], dt);

	if (q->pos[1] < 0.0)
	{
		if (q->hit < 2)
		{
			q->pos[1] = -q->pos[1];
			q->vel[1] *= -0.25f;
		}
		else
		{
			// final value (below ground)
			q->pos[1] = -1.0f;
			return false;
		}
		q->hit++;
	}

	return true;
}

void BloodDropEmitter::Render()
{
	glBindBuffer(GL_ARRAY_BUFFER_ARB, 0);
	glVertexPointer(4, GL_FLOAT, sizeof(BloodParticle), particle);
	glDrawArrays(GL_POINTS, 0, numParticles);
}
