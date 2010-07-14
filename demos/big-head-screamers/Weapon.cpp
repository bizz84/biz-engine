/*****************************************************************************
 * Filename			NBody.cpp
 * 
 * License			LGPL
 *
 * Author			Andrea Bizzotto (bizz84@gmail.com)
 *
 * Platform			LinuxX11 / OpenGL
 * 
 * Description		NBody Simulation
 *
 *****************************************************************************/

#include "Weapon.h"
#define _USE_MATH_DEFINES
#include <math.h>

#define M_RAD (180.0 / M_PI)
#define M_1_RAD (M_PI / 180.0)

const float Grenade::g = 2.0f;

Grenade::Grenade(const Point3 &p, const float yRot, const float xRot,
				 const float speed)
{
	bounces = 0;
	pos = -p;
	pos[1] -= 10.0f;
	vel = Matrix3::RotationY(-yRot) * (Matrix3::RotationX(-xRot) *
		Vector3(0.0f, 0.0f, -speed));
}


bool Grenade::Update(float dt)
{
	dt *= 20.0f;
	float f = g * dt;

	vel[1] -= f;

	pos += vel * dt;

	if (pos[1] < 0.0f)
	{
		bounces++;
		pos[1] = -pos[1];
		vel[0] *= 0.5f;
		vel[1] *= -0.5f;
		vel[2] *= 0.5f;
	}
	
	return !Impact();

}

const bool Grenade::Impact() const
{
	return bounces >= MAX_BOUNCES;/*pos.s[1] <= 0.0f;*/
}


bool GrenadeLauncher::Fire(const FPSCamera &cameraPos)
{
	if (canFire)
	{
		canFire = false;
		time = 0.0f;
		Grenade grenade(cameraPos.GetTranslation(),
			cameraPos.GetAlpha() * M_1_RAD, cameraPos.GetBeta() * M_1_RAD, 50.0f);
		grenades.push_back(grenade);
		return true;
	}
	return false;
}

bool Condition(const Grenade &grenade)
{
	return grenade.Impact();
}

void GrenadeLauncher::Update(const float dt)
{
	if ((time += dt) >= reloadTime && canFire == false)
	{
		canFire = true;
	}
	list<Grenade>::iterator iter;
	for (iter = grenades.begin(); iter != grenades.end(); iter++)
	{
		iter->Update(dt);
	}
	grenades.remove_if(Condition);
}
