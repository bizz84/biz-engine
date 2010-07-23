/*****************************************************************************
 * Filename			AIManager.h
 * 
 * License			LGPL
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
#include "Enemy.h"

#include <vector>

using namespace std;


// AIManager defines the generation and update logic of enemies.
// This is limited to state updates, while rendering happens in EnemyRenderer,
// which takes the input data vector as a parameter
class AIManager
{
	// Factory method for creating a new Enemy
	// TODO: modify to allow different types of enemies
	static Enemy *NewEnemy(const Vector2 &p, const int health,
		const int index1, const int index2
		) { return new SpriteEnemy(p, health, index1, index2); }

	// Vector containing all the enemies
	vector<Enemy *> data;
	
	// Spawn new enemy
	void Spawn(vector<Enemy *>::iterator &iter, const Vector2 &player);
public:
	AIManager(const Vector3 &player);
	~AIManager();

	// Update of all enemy positions
	void Input(const float t, const float dt, const Vector3 &player);
	// Remove enemies that have died (called after CollisionDetector::Run())
	void UpdateState(const Vector3 &player);
	
	// public const access (used for rendering)
	const vector<Enemy *> &GetData() const { return data; }

	// const values
	static const float ImpactDistance;
	static const float InitialMinDistance;
	static const unsigned int NumEnemies;
	static const float MaxDistance;

	static const float EnemyHeight;
	static const float EnemyRadius;
};

#endif