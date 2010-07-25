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
#ifndef _BULLET_H_
#define _BULLET_H_

#include "Extensions.h"
#include "Matrix.h"
#include "Settings.h"

/*****************************************************************************
 * Bullet class declaration
 *****************************************************************************/
 
class Bullet
{
protected:
	// store previous and current (needed for collision detection)
	Point3 pos[2];
	Vector3 vel;
	float xRot, yRot;
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
	const float GetAngleX() const { return xRot; }
	const float GetAngleY() const { return yRot; }

	virtual const unsigned int Damage() const = 0;
};

/*****************************************************************************
 * Grenade class declaration
 *****************************************************************************/

class Grenade : public Bullet
{
	unsigned int bounces;

public:
	Grenade(const Point3 &p, const float yRot, const float xRot,
		const float speed);
	virtual bool Update(const float dt);

	virtual const unsigned int Damage() const { return Settings::Instance().GrenadeDamage; }
};


#endif
