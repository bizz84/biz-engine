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
#ifndef _WEAPON_H_
#define _WEAPON_H_

#include "Extensions.h"
#include "Matrix.h"
#include "CameraController.h"

#include <vector>
#include <list>

using namespace std;

/*****************************************************************************
 * Bullet class declaration
 *****************************************************************************/
 
class Bullet
{
protected:
	// store previous and current (needed for collision detection)
	Point3 pos[2];
	Vector3 vel;
	bool hit;
public:
	Bullet(const Point3 &p, const float yRot, const float xRot,
		const float speed);
	virtual ~Bullet() { }
		
	virtual bool Update(const float dt) = 0;

	const bool Impact() const { return hit; }

	const Point3 GetPosition() const { return pos[1]; }
	const Point3 GetPrevPosition() const { return pos[0]; }
	
	virtual const unsigned int Damage() const = 0;
};

/*****************************************************************************
 * Grenade class declaration
 *****************************************************************************/

class Grenade : public Bullet
{
	unsigned int bounces;

	static const float g;
public:
	Grenade(const Point3 &p, const float yRot, const float xRot,
		const float speed);
	virtual bool Update(const float dt);

public:
	enum { MAX_BOUNCES = 3 };
	enum { DAMAGE = 50 };
	virtual const unsigned int Damage() const { return DAMAGE; }
};

/*****************************************************************************
 * BulletExplosion class declaration
 *****************************************************************************/
 
class BulletExplosion
{
	Point3 pos;
	float time;
public:
	BulletExplosion(const Point3 &p) : pos(p), time(0.0f) { }
	virtual ~BulletExplosion() { }
	
	bool Update(const float dt) { return (time += dt) < LifeTime(); }
	
	virtual const bool Ended() const { return time >= LifeTime(); }
	
	virtual const float LifeTime() const = 0;
};

/*****************************************************************************
 * GrenadeExplosion class declaration
 *****************************************************************************/

class GrenadeExplosion : public BulletExplosion
{

public:
	GrenadeExplosion(const Point3 &p) : BulletExplosion(p) { }
	
	virtual const float LifeTime() const { return 3.0f; }
};

/*****************************************************************************
 * WeaponSystem class declaration
 *****************************************************************************/

class WeaponSystem
{
public:
	enum WeaponType { TypeGrenade, TypePlasma, NumWeapons };
private:

	const float reloadTime;
	float time;
	// TODO: firing time depends on the weapon used
	bool canFire;

	WeaponType eCurrWeapon;

	// TODO: this should be a list of <Bullet *>
	list<Bullet *> bullets;
	// TODO: this should be a list of <Explosion *>
	list<BulletExplosion *> explosions;
public:
	WeaponSystem(const float reloadTime)
		: reloadTime(reloadTime), time(0.0f), canFire(true), eCurrWeapon(TypeGrenade) { }
	~WeaponSystem();

	Bullet *NewBullet(const Point3 &p, const float yRot, const float xRot,
		const float speed);

	// handles fire request
	bool Fire(const FPSCamera &cameraPos);
	void Update(const float dt);

	const list<Bullet *> &GetBullets() const { return bullets; }
	
	void AddExplosion(const Point3 &p) { explosions.push_back(new GrenadeExplosion(p)); }
};

#endif
