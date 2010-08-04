/*****************************************************************************
 * Filename			Weapon.h
 * 
 * License			GPLv3
 *
 * Author			Andrea Bizzotto (bizz84@gmail.com)
 *
 * Platform			LinuxX11 / OpenGL
 * 
 * Description		Weapon
 *
 *****************************************************************************/
#ifndef _ENEMY_RENDERER_ATTRIB_H_
#define _ENEMY_RENDERER_ATTRIB_H_

#include "Extensions.h"
#include "ProgramArray.h"
#include "Enemy.h"

#include <vector>
using namespace std;



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

	struct SpriteVertexData
	{
		Point2 pos;
		Point2 tex;
		
		float scale;
		float rotAngle;
		Vector3 translation;
		
		void SetAttributes(float s, float rot, const Vector3 &t)
		{
			scale = s;
			rotAngle = rot;
			translation = t;
		}
	};
	SpriteVertexData *attrib;

	bool LoadSprites();

	static Point2 TexCoord(Point2 coord, int index);

	bool SetAttribPointer(GLint loc, size_t size, GLenum type, void *address) const;
	bool UnsetAttribPointer(GLint loc) const;

public:
	EnemyRendererAttrib();
	~EnemyRendererAttrib();

	virtual bool Update(const ptr_vector<Enemy> &data, const float angle,
		const float height);

	virtual void Render(const ptr_vector<Enemy> &data, const float angle,
		const float height) const;	

	static const unsigned int NumSprites;
};

#endif
