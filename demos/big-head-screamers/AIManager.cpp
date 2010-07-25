/*****************************************************************************
 * Filename			AIManager.cpp
 * 
 * License			LGPL
 *
 * Author			Andrea Bizzotto (bizz84@gmail.com)
 *
 * Platform			LinuxX11 / OpenGL
 * 
 * Description		AIManager
 *
 *****************************************************************************/

#include "AIManager.h"
#include "Misc.h" // Used for RandRange()
#include "Enemy.h" // for NUM_SPRITES
#include "ParticleEmitter.h"
#include "Settings.h"

Enemy *AIManager::NewEnemy(const Vector2 &p, const int health,
		const int index1, const int index2)
{
	return new SpriteEnemy(p, health, index1, index2);
}

AIManager::AIManager(const Vector3 &player)
{
	float maxd = Settings::Instance().EnemyMaxDistance;
	// Create an array of enemies around the player
	const Vector2 target = Vector2(player[0], player[2]);
	for (unsigned int i = 0; i < Settings::Instance().NumEnemies; )
	{
		Vector2 pos = Vector2(RandRange(-maxd, maxd),
		                      RandRange(-maxd, maxd));
		// Minimum distance must be granted
		if ((pos - target).Length() < Settings::Instance().EnemyMinDistance)
			continue;
		// TODO: Used for sprite enemies. Move somewhere else in the factory.
		int texture = (rand() % (EnemyRenderer::NUM_SPRITES >> 1));
		data.push_back(NewEnemy(pos + target, Settings::Instance().EnemyHealth,
			texture << 1, (texture << 1) + 1));
		i++;
	}
}

AIManager::~AIManager()
{
	// deallocate memory
	// TODO: Use shared_ptr
	vector<Enemy *>::iterator iter;
	for (iter = data.begin(); iter != data.end(); iter++)
	{
		delete *iter;
	}

	list<ParticleEmitter *>::iterator e;
	for (e = particles.begin(); e != particles.end(); e++)
	{
		delete *e;
	}
}

void AIManager::Spawn(vector<Enemy *>::iterator &iter, const Vector2 &player)
{
	float maxd = Settings::Instance().EnemyMaxDistance;
	// Respawn and set health to maximum
	do
	{
		(*iter)->pos = player + Vector2(RandRange(-maxd, maxd),
                                     RandRange(-maxd, maxd));
	}
	while (((*iter)->pos - player).Length() < Settings::Instance().EnemyMinDistance);
	(*iter)->health = 100;
}


void AIManager::Input(const float t, const float dt, const Vector3 &player, bool apocalypse/* = false*/)
{
	const Vector2 target = Vector2(player[0], player[2]);
	vector<Enemy *>::iterator iter;

	// Update all enemies position
	for (iter = data.begin(); iter != data.end(); iter++)
	{
		// Update position
		Vector2 dir = target - (*iter)->pos;
		//(*iter)->hit = false;
		(*iter)->pos += dir.Normalize() * dt * Settings::Instance().EnemySpeed;
		// Check impact
		if (apocalypse || ((*iter)->pos - target).Length() < Settings::Instance().EnemyImpactDistance)
		{
			// Die, new one will spawn in UpdateState
			(*iter)->health = 0.0f;
		}
	}

	// Update particles
	// TODO: should be a generic Explosion * routine
	list<ParticleEmitter *>::iterator e;	
	for (e = particles.begin(); e != particles.end(); e++)
	{
		(*e)->Update(dt);
	}
}

// TODO: lambda function in C++0x
bool ExpiredCondition(const ParticleEmitter *particle)
{
	return particle->Expired();
}

void AIManager::AddParticles(const Point3 &pos, const unsigned int health)
{
	particles.push_back(new BloodDropEmitter(pos, Settings::Instance().NumBloodDrops));
}


void AIManager::UpdateState(const Vector3 &player)
{
	// Respawn dead enemies
	const Vector2 target = Vector2(player[0], player[2]);
	vector<Enemy *>::iterator iter;
	for (iter = data.begin(); iter != data.end(); iter++)
	{
		if ((*iter)->Dead())
		{
			// Some more blood never hurts
			Vector3 pos = Vector3((*iter)->pos[0], 0.75 * Settings::Instance().EnemyHeight, (*iter)->pos[1]);
			particles.push_back(new BloodDropEmitter(pos, Settings::Instance().NumBloodDrops));
			// New position
			Spawn(iter, target);
		}
	}
	particles.remove_if(ExpiredCondition);	
}