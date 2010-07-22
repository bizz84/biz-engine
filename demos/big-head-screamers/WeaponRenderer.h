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
#ifndef _WEAPON_RENDERER_H_
#define _WEAPON_RENDERER_H_

#include "Extensions.h"
#include "Weapon.h"
#include "VBO.h"
#include "ProgramArray.h"

class WeaponRenderer : private ProgramArray
{
	enum {
		P_LOOKUP_COLOR, 
		NUM_PROGRAMS 
	};

	IndexedVBO *pTetraVBO;
public:
	WeaponRenderer();
	~WeaponRenderer();
	void Render(const WeaponSystem *ws) const;
};

#endif
