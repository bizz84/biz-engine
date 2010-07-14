/*****************************************************************************
 * Filename			NBody.h
 * 
 * License			LGPL
 *
 * Author			Andrea Bizzotto (bizz84@gmail.com)
 *
 * Platform			LinuxX11 / OpenGL
 * 
 * Description		NBody Simulation
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

class Grenade
{
	unsigned int bounces;
	Point3 pos;
	Vector3 vel;

	static const float g;
public:
	Grenade(const Point3 &p, const float yRot, const float xRot,
		const float speed);
	bool Update(float dt);

	const bool Impact() const;

	const Point3 GetPosition() const { return pos; }
public:
	enum { MAX_BOUNCES = 3 };
};

class GrenadeLauncher
{
	const float reloadTime;
	float time;
	bool canFire;
	list<Grenade> grenades;
public:
	GrenadeLauncher(const float reloadTime)
		: reloadTime(reloadTime), time(0.0f), canFire(true) { }
	bool Fire(const FPSCamera &cameraPos);
	void Update(const float dt);

	const list<Grenade> &GetGrenades() const { return grenades; }
};

#endif