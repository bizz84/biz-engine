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

#include "Explosion.h"
#define _USE_MATH_DEFINES
#include <math.h>

#include "ParticleEmitter.h"

GrenadeExplosion::GrenadeExplosion(const Point3 &p)
 : BulletExplosion(p)
{
	emitter = new BloodDropEmitter(p, 200);
}

bool GrenadeExplosion::Update(const float dt)
{
	return emitter->Update(dt);
}


const bool GrenadeExplosion::Expired() const
{
	return emitter->Expired();
}

void GrenadeExplosion::Render()
{
	emitter->Render();

}