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
#include "Sprite2D.h"

#include <vector>
using namespace std;

// is-implemented-in-terms-of
class EnemyRenderer : private ProgramArray
{
public:
	// This needs to be accessible elsewhere
	enum { NUM_SPRITES = 8 };
private:
	enum EProgram {
		P_SPRITE,		// used for sprites
		P_SPRITE_ATTRIB, // used for instanced sprites
		P_TEST_ATTRIB,
		NUM_PROGRAMS
	};

	// Attributes used by the instancing shader
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

	GLuint uiSprite[NUM_SPRITES];

	SpriteVertexData *attrib;

	bool LoadSprites();

	bool SetAttribPointer(GLint loc, size_t size, GLenum type, void *address) const;
	bool UnsetAttribPointer(GLint loc) const;

public:
	EnemyRenderer();
	~EnemyRenderer();

	void Render(const vector<Enemy *> &data, const float angle, const float height) const;	

	static const unsigned int NumSprites;
};

#endif
