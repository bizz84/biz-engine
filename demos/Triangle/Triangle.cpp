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

class Triangle : public SDLShell
{

protected:
	Shader shader;
	GLuint uiProgram;

	float y, x, t;
	Timer timer;

	VBO *pTriangleVBO;

	Pointer pointer;

	virtual bool InitApp();
	virtual bool InitGL();
	virtual bool Render();
	virtual bool ReleaseGL();
	virtual bool ReleaseApp();	
	virtual bool Resize(unsigned int width, unsigned int height);

	virtual bool RequiresOpenGL2() { return true; }
	virtual bool RequiresTTF() { return true; }

	virtual const char *GetAppName() { return "HelloTriangle"; }
	virtual const char *GetAppVersion() { return "1.0.0"; }
public:
	Triangle();
	virtual ~Triangle();
};

Triangle::Triangle()
	: pTriangleVBO(NULL), pointer(this)
{
	t = x = y = 0.0f;

}

Triangle::~Triangle()
{

}

bool Triangle::InitApp()
{
	return true;

}

bool Triangle::InitGL()
{
	//shader.load("data/shaders/color.vert", "data/shaders/color.frag");
	GLResourceManager &loader = GLResourceManager::Instance();

	if (!loader.LoadShaderFromFile("data/shaders/color.vert", "data/shaders/color.frag", uiProgram))
    {
		return false;
    }
	printf("Loaded shader\n");


	float afAttribs[] = {
	  -50.0f, 0.0f, -40.0f, 1.0f, 0.0f, 0.0f,
	  50.0f, 0.0f, -40.0f, 0.0f, 1.0f, 0.0f,
	  0.0f, 0.0f, 60.0f, 0.0f, 0.0f, 1.0f
	};

	pTriangleVBO = new VBO(afAttribs, 6 * sizeof(float), 3);
	// Add all the arrays that need to be rendered
	pTriangleVBO->SetVertexData(0, 3);
	pTriangleVBO->SetColorData(sizeof(float) * 3, 3);
	// Add all the arrays that need to be rendered
	//pTriangleVBO->AddEntry(glVertexPointer, 3, GL_FLOAT, 0);
	//pTriangleVBO->AddEntry(glColorPointer, 3, GL_FLOAT, sizeof(float) * 3);


	Resize(ShellGet(SHELL_WIDTH), ShellGet(SHELL_HEIGHT));

	glDisable(GL_DEPTH_TEST);

	timer.Start();
	return true;
}

bool Triangle::Resize(unsigned int width, unsigned int height)
{
	//printf("Reshaping Screen to %dx%d\n", width, height);
	// Select and setup the projection matrix
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluPerspective( 65.0f, (GLfloat)width/(GLfloat)height, 1.0f, 1000.0f );
	return true;
}




bool Triangle::Render()
{
	pointer.Input();
	timer.Update();

	x += 0.002f * (GetPointer()->GetMotionX() + GetPointer()->GetInertiaX());
	y += 0.002f * (GetPointer()->GetMotionY() + GetPointer()->GetInertiaY());
	if (GetPointer()->TimeSinceLastInput() > 5.0f)
		t += timer.GetDeltaTime();


	// Clear color buffer
	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	// Select and setup the modelview matrix
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	gluLookAt( 0.0f, 1.0f, 0.0f,    // Eye-position
		       0.0f, 200.0f, 0.0f,   // View-point
		       0.0f, 0.0f, 1.0f );  // Up-vector

	// Draw a rotating colorful triangle
	glTranslatef( 0.0f, 190.0f, 0.0f );
	glRotatef( x + (GLfloat)t*30.0f, 0.0f, 0.0f, 1.0f );
	glRotatef( y, 0.0f, 1.0f, 0.0f );
	
	/* Draw triangle */
	shader.use();
	//glUseProgram(uiProgram);

	glBegin(GL_TRIANGLES);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(-50.0f, 0.0f, -40.0f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(50.0f, 0.0f, -40.0f);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, 60.0f);
	glEnd();

	/*glEnableClientState(GL_VERTEX_ARRAY);	
	glEnableClientState(GL_COLOR_ARRAY);
	pTriangleVBO->Render();
	glDisableClientState(GL_VERTEX_ARRAY);	
	glDisableClientState(GL_COLOR_ARRAY);*/


	//printf("%.1f\n",  1.0f / timer.GetDeltaTime());

	return true;
}

bool Triangle::ReleaseGL()
{
	delete pTriangleVBO;

	shader.remove();
	return GLResourceManager::Instance().Release();
}

bool Triangle::ReleaseApp()
{
	return true;
}

Shell *NewDemo()
{
	return new Triangle();
}

