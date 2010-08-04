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
#ifndef _WEAPON_MANAGER_H_
#define _WEAPON_MANAGER_H_

#include "Extensions.h"
#include "Matrix.h"
#include "CameraController.h"
#include "Misc.h"

#include "boost/ptr_container/ptr_list.hpp"
using namespace boost;

class Bullet;
class BulletRenderer;

/*****************************************************************************
 * WeaponManager class declaration
 *****************************************************************************/

class WeaponManager
{
public:
	enum WeaponType { TypeGrenade, TypeLaser, TypeTetra, NumWeapons };
private:

	float reloadTime[NumWeapons];
	float time;
	bool canFire;

	int currWeapon;

	// pimpl idiom
	ptr_list<Bullet> bullets;
	list<Bullet *> pList[NumWeapons];

	auto_ptr<BulletRenderer> pRenderer[NumWeapons];

public:
	WeaponManager();
	// TODO: why does this raise a compile error if defined on header file or
	// undefined at all?
	~WeaponManager();

	// Factory method
	void NewBullet(const Point3 &p, const float yRot, const float xRot,
		const float speed);

	// handles input
	void Input(const float dt, const FPSCamera &cameraPos, const bool fire);
	void UpdateState();

	// Get data array (used bt WeaponRenderer)
	// The non const version is the one passed to CollisionDetector
	ptr_list<Bullet> &GetBullets() { return bullets; }
	const ptr_list<Bullet> &GetBullets() const { return bullets; }

	void NextWeapon() { currWeapon = Next(currWeapon, NumWeapons); }
	void PrevWeapon() { currWeapon = Prev(currWeapon, NumWeapons); }
	const int CurrWeapon() const { return currWeapon; }

	void Render();
};

#endif
