/*****************************************************************************
 * Filename			AIManager.h
 * 
 * License			GPLv3
 *
 * Author			Andrea Bizzotto (bizz84@gmail.com)
 *
 * Platform			LinuxX11 / OpenGL
 * 
 * Description		AIManager defines the generation and update logic of
 *					enemies.
 *
 *****************************************************************************/
#ifndef _AI_MANAGER_H_
#define _AI_MANAGER_H_

#include "Extensions.h"
#include "Vector.h"

#include <vector>
#include <list>
using namespace std;

#include "boost/ptr_container/ptr_list.hpp"
#include "boost/ptr_container/ptr_vector.hpp"
using namespace boost;


class ParticleEmitter;
class Enemy;


// AIManager defines the generation and update logic of enemies.
// This is limited to state updates, while rendering happens in EnemyRenderer,
// which takes the input data vector as a parameter
class AIManager
{
	// Factory method for creating a new Enemy
	// TODO: modify to allow different types of enemies
	static Enemy *NewEnemy(const Vector2 &p, const int health,
		const int index1, const int index2);

	// Vector containing all the enemies
	ptr_vector<Enemy> data;

	// Blood
	ptr_list<ParticleEmitter> particles;

	// Spawn new enemy
	void Spawn(ptr_vector<Enemy>::iterator &iter, const Vector2 &player);
public:
	AIManager(const Vector3 &player);
	~AIManager();

	// Update of all enemy positions
	void Input(const float t, const float dt, const Vector3 &player, bool apocalypse = false);
	// Remove enemies that have died (called after CollisionDetector::Run())
	void UpdateState(const Vector3 &player);
	
	// public const access (used for rendering)
	ptr_vector<Enemy> &GetData() { return data; }
	const ptr_vector<Enemy> &GetData() const { return data; }

	const ptr_list<ParticleEmitter> &GetParticles() const { return particles; }

	void AddParticles(const Point3 &pos, const unsigned int health);
};

#endif