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


const float AIManager::ImpactDistance = 10.0f;
const float AIManager::InitialMinDistance = 100.0f;
const unsigned int AIManager::NumEnemies = 50;
const float AIManager::MaxDistance = 1000.0f;

const float AIManager::EnemyHeight = 20.0f;
const float AIManager::EnemyRadius = 15.0f;

const unsigned int AIManager::NumBloodDrops = 150;

Enemy *AIManager::NewEnemy(const Vector2 &p, const int health,
		const int index1, const int index2)
{
	return new SpriteEnemy(p, health, index1, index2);
}

AIManager::AIManager(const Vector3 &player)
{
	// Create an array of enemies around the player
	const Vector2 target = Vector2(player[0], player[2]);
	for (unsigned int i = 0; i < NumEnemies; )
	{
		Vector2 pos = Vector2(RandRange(-MaxDistance, MaxDistance),
		                      RandRange(-MaxDistance, MaxDistance));
		// Minimum distance must be granted
		if ((pos - target).Length() < InitialMinDistance)
			continue;
		// TODO: Used for sprite enemies. Move somewhere else in the factory.
		int texture = (rand() % (EnemyRenderer::NUM_SPRITES >> 1));
		data.push_back(NewEnemy(pos + target, 100,
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
	// Respawn and set health to maximum
	do
	{
		(*iter)->pos = player + Vector2(RandRange(-MaxDistance, MaxDistance),
                                     RandRange(-MaxDistance, MaxDistance));
	}
	while (((*iter)->pos - player).Length() < InitialMinDistance);
	(*iter)->health = 100;
}


void AIManager::Input(const float t, const float dt, const Vector3 &player)
{
	const Vector2 target = Vector2(player[0], player[2]);
	vector<Enemy *>::iterator iter;

	// Update all enemies position
	for (iter = data.begin(); iter != data.end(); iter++)
	{
		// Update position
		Vector2 dir = target - (*iter)->pos;
		//(*iter)->hit = false;
		(*iter)->pos += dir.Normalize() * dt * 20.0f;
		// Check impact
		if (((*iter)->pos - target).Length() < ImpactDistance)
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
	particles.push_back(new BloodDropEmitter(pos, NumBloodDrops));
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
			Vector3 pos = Vector3((*iter)->pos[0], 0.75 * EnemyHeight, (*iter)->pos[1]);
			particles.push_back(new BloodDropEmitter(pos, NumBloodDrops));
			// New position
			Spawn(iter, target);
		}
	}
	particles.remove_if(ExpiredCondition);	
}