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

#include "Weapon.h"
#define _USE_MATH_DEFINES
#include <math.h>

#define M_RAD (180.0 / M_PI)
#define M_1_RAD (M_PI / 180.0)

/*****************************************************************************
 * Bullet implementation
 *****************************************************************************/

 Bullet::Bullet(const Point3 &p, const float yRot, const float xRot,
		const float speed)
{
	impact = false;
	pos[1] = -p;
	pos[1][1] -= 10.0f;
	pos[0] = pos[1];
	vel = Matrix3::RotationY(-yRot) * (Matrix3::RotationX(-xRot) *
		Vector3(0.0f, 0.0f, -speed));		
}
		
/*****************************************************************************
 * Grenade implementation
 *****************************************************************************/
 
const float Grenade::g = 2.0f;

Grenade::Grenade(const Point3 &p, const float yRot, const float xRot,
				 const float speed) : Bullet(p, yRot, xRot, speed)
{
	bounces = 0;
}


bool Grenade::Update(float dt)
{
	pos[0] = pos[1];
	dt *= 20.0f;
	float f = g * dt;

	vel[1] -= f;

	pos[1] += vel * dt;

	if (pos[1][1] < 0.0f)
	{
		bounces++;
		pos[1][1] = -pos[1][1];
		vel[0] *= 0.5f;
		vel[1] *= -0.5f;
		vel[2] *= 0.5f;
	}
	if (bounces >= MAX_BOUNCES)
		impact = true;
	
	return !Impact();

}

/*****************************************************************************
 * WeaponSystem implementation
 *****************************************************************************/
WeaponSystem::~WeaponSystem()
{
	list<Bullet *>::iterator iter;
	for (iter = bullets.begin(); iter != bullets.end(); iter++)
	{
		delete *iter;
	}
	list<BulletExplosion *>::iterator e;
	for (e = explosions.begin(); e != explosions.end(); e++)
	{
		delete *e;
	}
}

Bullet *WeaponSystem::NewBullet(const Point3 &p, const float yRot,
								const float xRot, const float speed)
{
	//if (eCurrWeapon == Grenade)
	return new Grenade(p, yRot, xRot, speed);
}

bool ImpactCondition(const Bullet *bullet)
{
	return bullet->Impact();
}

bool ExplosionCondition(const BulletExplosion *explosion)
{
	return explosion->Expired();
}

void WeaponSystem::Input(const float dt, const FPSCamera &cameraPos, const bool fire)
{
	if ((time += dt) >= reloadTime && canFire == false)
	{
		canFire = true;
	}
	// User requested fire:
	if (fire && canFire)
	{
		canFire = false;
		time = 0.0f;
		// add
		bullets.push_back(NewBullet(cameraPos.GetTranslation(),
			cameraPos.GetAlpha() * M_1_RAD, cameraPos.GetBeta() * M_1_RAD, 50.0f));
	}

	// Update grenades
	// TODO: should be a generic Bullet * routine
	list<Bullet *>::iterator b;
	for (b = bullets.begin(); b != bullets.end(); b++)
	{
		(*b)->Update(dt);
	}

	
	// Update explosions
	// TODO: should be a generic Explosion * routine
	list<BulletExplosion *>::iterator e;	
	for (e = explosions.begin(); e != explosions.end(); e++)
	{
		(*e)->Update(dt);
	}

}

BulletExplosion *WeaponSystem::MakeExplosion(Bullet *b)
{
	string type = typeid(*b).name();
	if (type.find("Grenade") >= 0)
		return new GrenadeExplosion(b->GetPosition());
	return NULL;
}


void WeaponSystem::UpdateState()
{
	list<Bullet *>::iterator b;
	for (b = bullets.begin(); b != bullets.end(); b++)
	{
		if ((*b)->Impact())
		{
			AddExplosion((*b)->GetPosition());
			//printf("%s\n", typeid(*(*b)).name());
			//explosions.push_back(MakeExplosion(*b));
		}
	}
	// This is called after collision detection
	bullets.remove_if(ImpactCondition);
	explosions.remove_if(ExplosionCondition);	
}
