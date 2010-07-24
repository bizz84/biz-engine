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
#ifndef _EXPLOSION_H_
#define _EXPLOSION_H_

#include "Extensions.h"
#include "Matrix.h"
#include "CameraController.h"

#include <vector>
#include <list>

using namespace std;

class ParticleEmitter;

/*****************************************************************************
 * BulletExplosion class declaration
 *****************************************************************************/
 
class BulletExplosion
{
	Point3 pos;
public:
	BulletExplosion(const Point3 &p) : pos(p) { }
	virtual ~BulletExplosion() { }
	
	virtual bool Update(const float dt) { return true; }
	
	virtual const bool Expired() const = 0;

	virtual void Render() = 0;

	const Point3 &GetPosition() const { return pos; }
};

/*****************************************************************************
 * GrenadeExplosion class declaration
 *****************************************************************************/

class GrenadeExplosion : public BulletExplosion
{
	// composition
	ParticleEmitter *emitter;
public:
	GrenadeExplosion(const Point3 &p);
	
	virtual bool Update(const float dt);

	virtual const bool Expired() const;

	virtual void Render();
};


#endif
