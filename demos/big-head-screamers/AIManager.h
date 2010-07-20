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
	Enemy(const Vector2 &p, const int health = 100)
		: pos(p), health(health) { }
public:
	virtual ~Enemy() { }


	//const Vector2 &GetPos() const { return pos; }
	//const int Health() const { return health; }

	// public
	Vector2 pos;
	int health;
};

// AIManager defines the logic for the enemies and (currently) the rendering
// code as well (move this somewhere else?)
class AIManager
{
	class SpriteEnemy : public Enemy
	{
	public:
		SpriteEnemy(const Vector2 &p, const int index, const int health = 100)
				: Enemy(p, health), texIndex(index) { }

		int texIndex;
	};


	class EnemyFactory
	{
	public:
		static Enemy *NewEnemy(const Vector2 &p, const int index,
			const int health = 100) { return new SpriteEnemy(p, index, health); }
	};

	enum { NUM_SPRITES = 2 };
	GLuint uiSprite[NUM_SPRITES];

	vector<Enemy *> data;

	bool LoadSprites();
public:
	AIManager(const Vector3 &player);
	~AIManager();

	void Input(const float t, const float dt, const Vector3 &player);

	void Render(const float angleCorr);
	
	const vector<Enemy *> &GetData() const { return data; }

	static const float ImpactDistance;
	static const float InitialMinDistance;
	static const unsigned int NumEnemies;
	static const float MaxDistance;

	static const float EnemyHeight;
	static const float EnemyRadius;
};

#endif