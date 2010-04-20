/*****************************************************************************
 * Filename			Timer.cpp
 * 
 * License			LGPL
 *
 * Author			Andrea Bizzotto (bizz84@gmail.com)
 *
 * Platform			LinuxX11 / OpenGL
 * 
 * Description		High resolution timer
 *
 *****************************************************************************/

#include "CoordinateFrame.h"
#include "assert.h"

CoordinateFrame &CoordinateFrame::Instance()
{
	static CoordinateFrame cf;
	return cf;
}


CoordinateFrame::CoordinateFrame()
{
	bLoaded = false;
}

void CoordinateFrame::Make(const float scale)
{
	uiList = glGenLists(1);
	glNewList(uiList, GL_COMPILE);
		GLUquadric *arrow0 = gluNewQuadric();
		gluDisk(arrow0, 0.0f, scale * 0.05f, 10, 1);
	
		GLUquadric *arrow1 = gluNewQuadric();
		gluCylinder(arrow1, scale * 0.05f, scale * 0.05f, scale * 0.8f, 10, 1);
	
		glPushMatrix();
		glTranslatef(0.0f, 0.0f, scale * 0.8f);
		GLUquadric *arrow2 = gluNewQuadric();
		gluDisk(arrow2, scale * 0.05f, scale * 0.1f, 10, 1);
	
		GLUquadric *arrow3 = gluNewQuadric();
		gluCylinder(arrow3, scale * 0.1f, 0.0f, scale * 0.2f, 10, 1);
		glPopMatrix();
	glEndList();
	bLoaded = true;
}

void CoordinateFrame::Render()
{
	assert(bLoaded);

	glPushMatrix();
	glScalef(0.1f, 0.1f, 0.1f);
	glColor3f(1.0f, 0.0f, 0.0f);
	glCallList(uiList);

	glPushMatrix();
	glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glCallList(uiList);
	glPopMatrix();

	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	glColor3f(0.0f, 0.0f, 1.0f);
	glCallList(uiList);

	glPopMatrix();
}

void CoordinateFrame::Unload()
{
	if (glIsList(uiList))
		glDeleteLists(uiList, 1);
}