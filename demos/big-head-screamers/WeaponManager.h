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
#include "Explosion.h"

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
	bool impact;
public:
	Bullet(const Point3 &p, const float yRot, const float xRot,
		const float speed);
	virtual ~Bullet() { }
		
	virtual bool Update(const float dt) = 0;

	const bool Impact() const { return impact; }

	const Point3 GetPosition() const { return pos[1]; }
	const Point3 GetPrevPosition() const { return pos[0]; }
	
	void SetImpact() { impact = true; }
	//void SetPosition(const Point3 &pos) { pos[1] = pos; }

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

	list<Bullet *> bullets;
	list<BulletExplosion *> explosions;
	
	BulletExplosion *MakeExplosion(Bullet *b);
public:
	WeaponSystem(const float reloadTime)
		: reloadTime(reloadTime), time(0.0f), canFire(true), eCurrWeapon(TypeGrenade) { }
	~WeaponSystem();

	// Factory method
	Bullet *NewBullet(const Point3 &p, const float yRot, const float xRot,
		const float speed);

	// handles input
	void Input(const float dt, const FPSCamera &cameraPos, const bool fire);
	void UpdateState();

	// Get data array (used bt WeaponRenderer)
	const list<Bullet *> &GetBullets() const { return bullets; }
	const list<BulletExplosion *> &GetExplosions() const { return explosions; }
	
	void AddExplosion(const Point3 &p) { explosions.push_back(new GrenadeExplosion(p)); }
};

#endif
