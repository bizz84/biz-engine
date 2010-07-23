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

/*****************************************************************************
 * Base EnemyRenderer class for rendering billboarded enemies
 *****************************************************************************/

class EnemyRenderer
{
public:
	// This needs to be accessible elsewhere
	enum { NUM_SPRITES = 8 };
public:
	virtual ~EnemyRenderer() { }
	virtual bool LoadSprites() = 0;
	virtual void Render(const vector<Enemy *> &data, const float angle,
		const float height) const = 0;
};

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

/*****************************************************************************
 * Derived class (one draw call for all enemies via instancing)
 *****************************************************************************/

// is-implemented-in-terms-of
class EnemyRendererAttrib : public EnemyRenderer, private ProgramArray
{
public:
private:
	enum EProgram {	P_SPRITE_ATTRIB, NUM_PROGRAMS };

	// Attributes used by the instancing shader
	enum {
		A_VERTEX,
		A_TEX_COORD,
		A_SCALE,
		A_ROT_ANGLE,
		A_TRANSLATE,
		P_ATTRIBS
	};
	GLint attribLoc[P_ATTRIBS];

	GLuint UseProgram(int index) const;

	GLuint uiAtlas;

	SpriteVertexData *attrib;

	bool LoadSprites();

	static Point2 TexCoord(Point2 coord, int index);

	bool SetAttribPointer(GLint loc, size_t size, GLenum type, void *address) const;
	bool UnsetAttribPointer(GLint loc) const;

public:
	EnemyRendererAttrib();
	~EnemyRendererAttrib();

	virtual void Render(const vector<Enemy *> &data, const float angle, const float height) const;	

	static const unsigned int NumSprites;
};

#endif
