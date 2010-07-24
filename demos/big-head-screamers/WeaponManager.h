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
#ifndef _WEAPON_MANAGER_H_
#define _WEAPON_MANAGER_H_

#include "Extensions.h"
#include "Matrix.h"
#include "CameraController.h"

#include <list>

using namespace std;

class Bullet;

/*****************************************************************************
 * WeaponManager class declaration
 *****************************************************************************/

class WeaponManager
{
public:
	enum WeaponType { TypeGrenade, TypePlasma, NumWeapons };
private:

	const float reloadTime;
	float time;
	// TODO: firing time depends on the weapon used
	bool canFire;

	WeaponType eCurrWeapon;

	list<Bullet *> bullets;
	
public:
	WeaponManager(const float reloadTime)
		: reloadTime(reloadTime), time(0.0f), canFire(true), eCurrWeapon(TypeGrenade) { }
	~WeaponManager();

	// Factory method
	Bullet *NewBullet(const Point3 &p, const float yRot, const float xRot,
		const float speed);

	// handles input
	void Input(const float dt, const FPSCamera &cameraPos, const bool fire);
	void UpdateState();

	// Get data array (used bt WeaponRenderer)
	const list<Bullet *> &GetBullets() const { return bullets; }	
};

#endif
