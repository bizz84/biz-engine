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
#include "AIManager.h"
#include "ProgramArray.h"

// is-implemented-in-terms-of
class EnemyRenderer : private ProgramArray
{
	enum EProgram {
		P_SPRITE,		// used for sprites
		P_SPRITE_ATTRIB, // used for instanced sprites
		NUM_PROGRAMS
	};

	enum {
		A_VERTEX,
		A_TEX_COORD,
		A_TEX_INDEX,
		A_SCALE,
		A_ROT_ANGLE,
		A_TRANSLATE,
		P_ATTRIBS
	};
	GLint attribLoc[P_ATTRIBS];

	GLuint UseProgram(EProgram index) const;

	enum { NUM_SPRITES = 8 };
	GLuint uiSprite[NUM_SPRITES];

	bool LoadSprites();

public:
	EnemyRenderer();

	void Render(const AIManager *ai, const float angle, const float height) const;
	
};

#endif
