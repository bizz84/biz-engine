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
#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include <stdio.h>
#include "Extensions.h"

class Settings
{
protected:
	bool Read();
	bool Write();
	Settings();
public:
	// sigleton, constant
	static const Settings &Instance();

	// data fields
	unsigned int NumEnemies;
	float EnemyImpactDistance;
	float EnemyMinDistance;
	float EnemyMaxDistance;

	float EnemyHeight;
	float CollisionRadius;
	unsigned int EnemyHealth;
	float EnemySpeed;
	float EnemyScale;

	unsigned int NumBloodDrops;
	float ParticleGravity;
	float ParticleSpeed;
	float PointSize;

	// Near, Far values for projection matrix and distance from plane
	float Fov;
	float Near;
	float Far;

	// Demo timing values for changing skybox and ground textures
	float MinRandomCycle;
	float MaxRandomCycle;
	float SkyBoxTransitionTime;

	// From Bullet
	float BulletGravity;
	float BulletSpeed;
	float GrenadeSize;
	unsigned int GrenadeDamage;
	unsigned int GrenadeMaxBounces;
	float GrenadeBounceSlowDown;
	float GrenadeReload;

	float LaserSpeed;
	float LaserReload;
	unsigned int LaserDamage;
	float LaserMaxDistance;

};

#endif
