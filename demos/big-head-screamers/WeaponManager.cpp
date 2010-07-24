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

#include "WeaponManager.h"
#include "Bullet.h"

#define _USE_MATH_DEFINES
#include <math.h>

#define M_RAD (180.0 / M_PI)
#define M_1_RAD (M_PI / 180.0)

/*****************************************************************************
 * WeaponManager implementation
 *****************************************************************************/
WeaponManager::~WeaponManager()
{
	list<Bullet *>::iterator iter;
	for (iter = bullets.begin(); iter != bullets.end(); iter++)
	{
		delete *iter;
	}

}

Bullet *WeaponManager::NewBullet(const Point3 &p, const float yRot,
								const float xRot, const float speed)
{
	//if (eCurrWeapon == Grenade)
	return new Grenade(p, yRot, xRot, speed);
}

bool ImpactCondition(const Bullet *bullet)
{
	return bullet->Impact();
}


void WeaponManager::Input(const float dt, const FPSCamera &cameraPos, const bool fire)
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
}



void WeaponManager::UpdateState()
{
	// This is called after collision detection
	bullets.remove_if(ImpactCondition);
}
