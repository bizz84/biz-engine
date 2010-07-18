/*****************************************************************************
 * Filename			Sprite2D.h
 * 
 * License			LGPL
 *
 * Author			Andrea Bizzotto (bizz84@gmail.com)
 *
 * Platform			LinuxX11 / OpenGL
 * 
 * Description		Sprite2D
 *
 *****************************************************************************/
#ifndef _SPRITE_2D_H_
#define _SPRITE_2D_H_

#include "Extensions.h"

#include "Vector.h"

class Sprite2D
{

public:
	Sprite2D();

	static void Render(const Vector2 &planePos, const float height,
					  const float scale, const float angleCorr);

};
#endif