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
#ifndef _ENEMY_H_
#define _ENEMY_H_

#include "Vector.h"

#include "boost/ptr_container/ptr_vector.hpp"
using namespace boost;

/*****************************************************************************
 * class defining an enemy
 *****************************************************************************/

class Enemy
{
protected:
	// Only AIManager can create instances of Enemy via derived classes
	Enemy(const Vector2 &p, const int health)
		: pos(p), health(health) { }
public:
	virtual ~Enemy() { }

	// public
	Vector2 pos;
	int health;

	bool Dead() const { return health <= 0; }
};

/*****************************************************************************
 * Sprite-type enemy (adds the texIndex depending on health)
 *****************************************************************************/

class SpriteEnemy : public Enemy
{
public:
	SpriteEnemy(const Vector2 &p, const int health,
		const int index1, const int index2)
			: Enemy(p, health), texIndex0(index1), texIndex1(index2) { }

	int texIndex0, texIndex1;

	const int GetTextureIndex() const { return health <= 50 ? texIndex1 : texIndex0; }
};


/*****************************************************************************
 * Base EnemyRenderer class for rendering billboarded enemies
 *****************************************************************************/

class EnemyRenderer
{
public:
	// This needs to be accessible elsewhere
	enum { NUM_SPRITES = 10 };
public:
	virtual ~EnemyRenderer() { }
	virtual bool LoadSprites() = 0;
	virtual bool Update(const ptr_vector<Enemy> &data, const float angle,
		const float height) { return false; }
	virtual void Render(const ptr_vector<Enemy> &data, const float angle,
		const float height) const = 0;
};

#endif
