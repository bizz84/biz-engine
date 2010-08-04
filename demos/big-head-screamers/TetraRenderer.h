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
#ifndef _TETRA_RENDERER_H_
#define _TETRA_RENDERER_H_

#include "Extensions.h"
#include "ProgramArray.h"
#include "BulletRenderer.h"

#include <list>
using namespace std;

class Bullet;
class IndexedVBO;

class TetraRenderer : public BulletRenderer, private ProgramArray
{
	enum { P_LOOKUP_COLOR, NUM_PROGRAMS };

	IndexedVBO *pTetraVBO;
public:
	TetraRenderer();
	~TetraRenderer();
	virtual void Render(const list<Bullet *> &bullets) const;
};

#endif
