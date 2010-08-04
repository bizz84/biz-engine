/*****************************************************************************
 * Filename			Weapon.cpp
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

#include "WeaponManager.h"
#include "Bullet.h"
#include "GrenadeRenderer.h"
#include "LaserRenderer.h"
#include "TetraRenderer.h"

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
	reloadTime[TypeTetra] = Settings::Instance().LaserReload;

	
	pRenderer[TypeGrenade] = auto_ptr<BulletRenderer>(new GrenadeRenderer());
	pRenderer[TypeLaser] = auto_ptr<BulletRenderer>(new LaserRenderer());
	pRenderer[TypeTetra] = auto_ptr<BulletRenderer>(new TetraRenderer());

}	


WeaponManager::~WeaponManager()
{

}

void WeaponManager::NewBullet(const Point3 &p, const float yRot,
								const float xRot, const float speed)
{
	Bullet *bullet;
	switch (CurrWeapon())
	{
	case TypeGrenade:
		bullet = new GravityBullet(p, yRot, xRot, speed);
		break;
	// Both of these have the same update logic
	case TypeLaser:
	case TypeTetra:
	default:
		bullet = new StraightBullet(p, yRot, xRot, speed);
		break;
	}
	pList[CurrWeapon()].push_back(bullet);
	bullets.push_back(bullet);
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
		NewBullet(cameraPos.GetTranslation(),
			cameraPos.GetAlpha() * M_1_RAD, cameraPos.GetBeta() * M_1_RAD, 50.0f);
	}

	// Update grenades
	// TODO: should be a generic Bullet * routine
	ptr_list<Bullet>::iterator b;
	for (b = bullets.begin(); b != bullets.end(); b++)
	{
		b->Update(dt);
	}
}

bool RemoveCondition(const Bullet *bullet)
{
	return bullet->Impact();
}

bool EraseCondition(const Bullet &bullet)
{
	return bullet.Impact();
}


void WeaponManager::UpdateState()
{
	// This is called after collision detection
	for (unsigned int i = 0; i < NumWeapons; i++)
	{
		pList[i].remove_if(RemoveCondition);
	}
	bullets.erase_if(EraseCondition);
}

void WeaponManager::Render()
{
	pRenderer[TypeGrenade]->Render(pList[TypeGrenade]);
	pRenderer[TypeTetra]->Render(pList[TypeTetra]);
	glEnable(GL_BLEND);
	pRenderer[TypeLaser]->Render(pList[TypeLaser]);
	glDisable(GL_BLEND);
}
