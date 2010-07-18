/*****************************************************************************
 * Filename			Sprite2D.cpp
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

#include "Sprite2D.h"
#include "Misc.h"

Sprite2D::Sprite2D()
{

}

void Sprite2D::Render(const Vector2 &planePos, const float height,
					  const float scale, const float angleCorr)
{
	glPushMatrix();

	glTranslatef(planePos[0], height, planePos[1]);
	glRotatef(angleCorr, 0.0f, 1.0f, 0.0f);
	glScalef(scale, scale, scale);

	RenderQuad2D(-0.5f, -1.0f, 1.0f, 2.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	glPopMatrix();
}
