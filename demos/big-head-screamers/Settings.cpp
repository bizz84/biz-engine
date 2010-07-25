/*****************************************************************************
 * Filename			Settings.cpp
 * 
 * License			LGPL
 *
 * Author			Andrea Bizzotto (bizz84@gmail.com)
 *
 * Platform			LinuxX11 / OpenGL
 * 
 * Description		Class for reading and writing settings to a file
 *
 *****************************************************************************/

#include "Settings.h"

#include <fstream>
using namespace std;

const Settings &Settings::Instance()
{
	static Settings settings;
	return settings;
}

Settings::Settings() :
	NumEnemies(100),
	EnemyImpactDistance(10.0f),
	EnemyMinDistance(100.0f),
	EnemyMaxDistance(1000.0f),

	EnemyHeight(20.0f),
	CollisionRadius(15.0f),
	EnemyHealth(100),
	EnemySpeed(20.0f),

	NumBloodDrops(200),
	ParticleGravity(100.0f),
	ParticleSpeed(15.0f),
	PointSize(3.5f),

	// Near, Far values for projection matrix and distance from plane
	Fov(90.0f),
	Near(1.0f),
	Far(100000.0f),

	// Demo timing values for changing skybox and ground textures
	MinRandomCycle(7.5f),
	MaxRandomCycle(15.0f),
	SkyBoxTransitionTime(2.0f),

	// From Bullet
	BulletGravity(2.0f),
	BulletSpeed(12.0f),
	GrenadeSize(1.5f),
	GrenadeDamage(50),
	GrenadeMaxBounces(3),
	GrenadeBounceSlowDown(0.5f)
{
	// Read from configuration file or write it
	if (!Read())
		Write();
}


#define READ(S, X, NAME) \
	S >> X; \
	if (string(X).compare(#NAME)) \
		return false; \
	S >> NAME;

#define WRITE(X) \
	<< #X << " " << X << "\n"

bool Settings::Read()
{
	char fieldName[30];
	fstream stream("data/settings.txt", ios_base::in);
	if (stream)
	{
		READ(stream, fieldName, NumEnemies)
		READ(stream, fieldName, EnemyImpactDistance)
		READ(stream, fieldName, EnemyMinDistance)
		READ(stream, fieldName, EnemyMaxDistance)
		READ(stream, fieldName, EnemyHeight)
		READ(stream, fieldName, CollisionRadius)
		READ(stream, fieldName, EnemyHealth)
		READ(stream, fieldName, EnemySpeed)
		READ(stream, fieldName, NumBloodDrops)
		READ(stream, fieldName, ParticleGravity)
		READ(stream, fieldName, ParticleSpeed)
		READ(stream, fieldName, PointSize)
		READ(stream, fieldName, Fov)
		READ(stream, fieldName, Near)
		READ(stream, fieldName, Far)
		READ(stream, fieldName, MinRandomCycle)
		READ(stream, fieldName, MaxRandomCycle)
		READ(stream, fieldName, SkyBoxTransitionTime)
		READ(stream, fieldName, BulletGravity)
		READ(stream, fieldName, BulletSpeed)
		READ(stream, fieldName, GrenadeSize)
		READ(stream, fieldName, GrenadeDamage)
		READ(stream, fieldName, GrenadeMaxBounces)
		READ(stream, fieldName, GrenadeBounceSlowDown)
		return true;
	}
	return false;
}

bool Settings::Write()
{
	fstream stream("data/settings.txt", ios_base::out);
	if (!stream)
		return false;

	stream
		WRITE(NumEnemies)
		WRITE(EnemyImpactDistance)
		WRITE(EnemyMinDistance)
		WRITE(EnemyMaxDistance)
		WRITE(EnemyHeight)
		WRITE(CollisionRadius)
		WRITE(EnemyHealth)
		WRITE(EnemySpeed)
		WRITE(NumBloodDrops)
		WRITE(ParticleGravity)
		WRITE(ParticleSpeed)
		WRITE(PointSize)
		WRITE(Fov)
		WRITE(Near)
		WRITE(Far)
		WRITE(MinRandomCycle)
		WRITE(MaxRandomCycle)
		WRITE(SkyBoxTransitionTime)
		WRITE(BulletGravity)
		WRITE(BulletSpeed)
		WRITE(GrenadeSize)
		WRITE(GrenadeDamage)
		WRITE(GrenadeMaxBounces)
		WRITE(GrenadeBounceSlowDown);

	return true;
}
