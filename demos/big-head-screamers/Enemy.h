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

#endif
