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
#ifndef _LASER_RENDERER_H_
#define _LASER_RENDERER_H_

#include "Extensions.h"
#include "ProgramArray.h"
#include "BulletRenderer.h"

#include "Vector.h"

#include <list>
using namespace std;

class Bullet;

// This uses a pseudo instancing approach (uniform batch)
// TODO: Document
class LaserRenderer : public BulletRenderer, private ProgramArray
{
	enum { P_LASER, NUM_PROGRAMS };

	enum { BATCH_SIZE = 64 };

	GLuint uiTexture;
	GLint iColorLoc;
	GLint iTranslateLoc;
	GLint iRotateLoc;

	
	//Matrix3 rotUni[64];
	//Vector2 rotUni[64];
	//Vector3 trUni[64];

public:
	LaserRenderer();
	~LaserRenderer() { }
	virtual void Render(const list<Bullet *> &bullets) const;
};

#endif
