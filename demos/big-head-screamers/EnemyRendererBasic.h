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
#ifndef _ENEMY_RENDERER_H_
#define _ENEMY_RENDERER_H_

#include "Extensions.h"
#include "ProgramArray.h"
#include "Enemy.h"

#include <vector>
using namespace std;



/*****************************************************************************
 * Derived class (one draw call per enemy)
 *****************************************************************************/

class EnemyRendererBasic : public EnemyRenderer, private ProgramArray
{
	enum EProgram { P_SPRITE, NUM_PROGRAMS };

	GLuint uiSprite[NUM_SPRITES];
public:
	EnemyRendererBasic();
	virtual bool LoadSprites();
	virtual void Render(const vector<Enemy *> &data, const float angle,
		const float height) const;

};

#endif
