/*****************************************************************************
 * Filename			AIManager.h
 * 
 * License			LGPL
 *
 * Author			Andrea Bizzotto (bizz84@gmail.com)
 *
 * Platform			LinuxX11 / OpenGL
 * 
 * Description		AIManager defines the logic for the enemies and (currently)
 *                  the rendering code as well.
 *
 *****************************************************************************/
#ifndef _AI_MANAGER_H_
#define _AI_MANAGER_H_

#include "Extensions.h"
#include "Vector.h"

#include <vector>

using namespace std;

class Enemy
{
protected:
	// Only AIManager can create instances of Enemy via derived classes
	Enemy(const Vector2 &p, const int health)
		: pos(p), health(health) { }
public:
	virtual ~Enemy() { }


	//const Vector2 &GetPos() const { return pos; }
	//const int Health() const { return health; }

	// public
	Vector2 pos;
	int health;

	bool Dead() const { return health <= 0; }
};

class SpriteEnemy : public Enemy
{
public:
	SpriteEnemy(const Vector2 &p, const int health,
		const int index1, const int index2)
			: Enemy(p, health), texIndex0(index1), texIndex1(index2) { }

	int texIndex0, texIndex1;

	int GetTextureIndex() { return health <= 50 ? texIndex1 : texIndex0; }
};

// AIManager defines the logic for the enemies and (currently) the rendering
// code as well (move this somewhere else?)
class AIManager
{
	// Factory method
	static Enemy *NewEnemy(const Vector2 &p, const int health,
		const int index1, const int index2
		) { return new SpriteEnemy(p, health, index1, index2); }

	enum { NUM_SPRITES = 8 };

	vector<Enemy *> data;
	
	void Spawn(vector<Enemy *>::iterator &iter, const Vector2 &player);
public:
	AIManager(const Vector3 &player);
	~AIManager();

	void Input(const float t, const float dt, const Vector3 &player);
	void UpdateState(const Vector3 &player);
	
	const vector<Enemy *> &GetData() const { return data; }

	static const float ImpactDistance;
	static const float InitialMinDistance;
	static const unsigned int NumEnemies;
	static const float MaxDistance;

	static const float EnemyHeight;
	static const float EnemyRadius;
};

#endif