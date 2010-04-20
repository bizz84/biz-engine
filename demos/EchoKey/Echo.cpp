/*****************************************************************************
 * Filename			Shadows.cpp
 * 
 * License			LGPL
 *
 * Author			Andrea Bizzotto (bizz84@gmail.com)
 *
 * Platform			LinuxX11 / OpenGL
 * 
 * Description		Lighting & Shadows demo using stencil buffers
 *
 *****************************************************************************/

#include "Extensions.h"
#include "SDLShell.h"
#include "Misc.h"
#include "Timer.h"
#include "Pointer.h"

#include <math.h>
#include <stdio.h>

#include "shader.h"
#include "GLResourceManager.h"
#include "VBO.h"

using namespace std;

class Echo : public SDLShell
{

protected:



public:
	Echo();
	virtual ~Echo();
	virtual bool InitApp();
	virtual bool InitGL();
	virtual bool Render();
	virtual bool ReleaseGL();
	virtual bool ReleaseApp();	
	virtual bool Resize(unsigned int width, unsigned int height);

	virtual bool RequiresOpenGL2() { return false; }
	virtual bool RequiresTTF() { return false; }
};

Echo::Echo()
{


}

Echo::~Echo()
{

}

bool Echo::InitApp()
{
	return true;

}

bool Echo::InitGL()
{

	return true;
}

bool Echo::Resize(unsigned int width, unsigned int height)
{
	//printf("Reshaping Screen to %dx%d\n", width, height);
	// Select and setup the projection matrix
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluPerspective( 65.0f, (GLfloat)width/(GLfloat)height, 1.0f, 1000.0f );
	return true;
}




bool Echo::Render()
{


	return true;
}

bool Echo::ReleaseGL()
{
	return true;
}

bool Echo::ReleaseApp()
{
	return true;
}

Shell *NewDemo()
{
	return new Echo();
}

