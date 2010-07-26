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
#include "GrenadeRenderer.h"
#include "LaserRenderer.h"

#define _USE_MATH_DEFINES
#include <math.h>

#define M_RAD (180.0 / M_PI)
#define M_1_RAD (M_PI / 180.0)

/*****************************************************************************
 * WeaponManager implementation
 *****************************************************************************/
WeaponManager::WeaponManager()
		: time(0.0f), canFire(true), currWeapon(TypeGrenade)
{
	reloadTime[TypeGrenade] = Settings::Instance().GrenadeReload;
	reloadTime[TypeLaser] = Settings::Instance().LaserReload;

	// Initialize GrenadeRenderer
	pGrenadeRenderer = new GrenadeRenderer();
	pLaserRenderer = new LaserRenderer();
}	


WeaponManager::~WeaponManager()
{

}

Bullet *WeaponManager::NewBullet(const Point3 &p, const float yRot,
								const float xRot, const float speed)
{
	switch (CurrWeapon())
	{
	case TypeGrenade:
		return new Grenade(pGrenadeRenderer, p, yRot, xRot, speed);
	case TypeLaser:
	default:
		return new Laser(pLaserRenderer, p, yRot, xRot, speed);
	}
}



void WeaponManager::Input(const float dt, const FPSCamera &cameraPos, const bool fire)
{
	if ((time += dt) >= reloadTime[CurrWeapon()] && canFire == false)
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
	ptr_list<Bullet>::iterator b;
	for (b = bullets.begin(); b != bullets.end(); b++)
	{
		b->Update(dt);
	}
}

bool ImpactCondition(const Bullet &bullet)
{
	return bullet.Impact();
}


void WeaponManager::UpdateState()
{
	// FIXME: This causes memory leaks!!
	// This is called after collision detection
	bullets.erase_if(ImpactCondition);
}

void WeaponManager::Render()
{
	// Grout items, then render them via appropriate renderer
	//list<Bullet *> grenade;
	//list<Bullet *> laser;


	ptr_list<Bullet>::iterator b;
	for (b = bullets.begin(); b != bullets.end(); b++)
	{
		b->Render();
	}

	//pGrenadeRenderer->Render(grenade);
	//pLaserRenderer->Render(laser);

}
