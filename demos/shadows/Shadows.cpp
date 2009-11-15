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

#include <GL/gl.h>
#include "SDLShell.h"
#include "Misc.h"
#include "Timer.h"
#include "TTFont.h"
#include "Pointer.h"

#include <math.h>
#include <stdio.h>

#include "GLResourceManager.h"
#include "VBO.h"

#include <vector>

using namespace std;

enum Shaders { E_LOOKUP, E_COLOR, E_UNIFORM };

class Shadows : public SDLShell
{

protected:
	GLuint uiProgram[3];

	float tRelease;
	float y, x, t;
	Timer timer;

	TTFont ttFont;

	GLuint uiCheckerTexture;

	VBO *pTriangleVBO;
	IndexedVBO *pMonkeyVBO;

	Pointer pointer;


public:
	Shadows();
	virtual ~Shadows();
	virtual bool InitApp();
	virtual bool InitGL();
	virtual bool Render();
	virtual bool ReleaseGL();
	virtual bool ReleaseApp();	
	virtual bool Resize(unsigned int width, unsigned int height);
};

Shadows::Shadows()
	: pTriangleVBO(NULL), pMonkeyVBO(NULL), pointer(this)
{
	t = x = y = 0.0f;
	tRelease = 0.0f;
}

Shadows::~Shadows()
{

}

bool Shadows::InitApp()
{
	return true;

}

bool Shadows::InitGL()
{
	GLResourceManager &loader = GLResourceManager::Instance();
	if (!loader.LoadShaderFromFile("data/shaders/Lookup.vert", "data/shaders/Lookup.frag", uiProgram[E_LOOKUP]))
    {
		return false;
    }

	if (!loader.LoadShaderFromFile("data/shaders/color.vert", "data/shaders/color.frag", uiProgram[E_COLOR]))
    {
		return false;
    }

	if (!loader.LoadShaderFromFile("data/shaders/uniform.vert", "data/shaders/uniform.frag", uiProgram[E_UNIFORM]))
    {
		return false;
    }

	if (!ttFont.LoadFont("data/fonts/LucidaBrightDemiBold.ttf", 20))
		return false;
	

	if (!loader.LoadTextureFromFile("data/textures/perspec_warp_input_tex.bmp", uiCheckerTexture, GL_LINEAR, GL_LINEAR))
		return false;

	float afAttribs[] = {
	  -5.0f, 0.0f, -4.0f, 1.0f, 0.0f, 0.0f,
	  5.0f, 0.0f, -4.0f, 0.0f, 1.0f, 0.0f,
	  0.0f, 0.0f, 6.0f, 0.0f, 0.0f, 1.0f
	};

	pTriangleVBO = new VBO(afAttribs, 6 * sizeof(float), 3);

	// Add all the arrays that need to be rendered
	pTriangleVBO->AddEntry(glVertexPointer, 3, GL_FLOAT, 0);
	pTriangleVBO->AddEntry(glColorPointer, 3, GL_FLOAT, sizeof(float) * 3);
	
	unsigned int index;
	if (!loader.Load3DSFile("data/scenes/monkey.3ds", index))
		return false;

	if (!loader.LoadMeshVBO(index, "Suzanne", pMonkeyVBO))
		return false;
	

	Resize(ShellGet(SHELL_WIDTH), ShellGet(SHELL_HEIGHT));

	glDisable(GL_DEPTH_TEST);

	timer.Start();
	return true;
}

bool Shadows::Resize(unsigned int width, unsigned int height)
{
	//printf("Reshaping Screen to %dx%d\n", width, height);
	// Select and setup the projection matrix
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluPerspective( 65.0f, (GLfloat)width/(GLfloat)height, 1.0f, 100.0f );
	return true;
}




bool Shadows::Render()
{
	pointer.Input();
	timer.Update();

	if (GetPointer()->Released())
	{
		tRelease = timer.GetTime();
	}

	x += 0.002f * (GetPointer()->GetMotionX() + GetPointer()->GetInertiaX());
	y += 0.002f * (GetPointer()->GetMotionY() + GetPointer()->GetInertiaY());
	if (GetPointer()->TimeSinceLastInput() > 5.0f)
		t += timer.GetDeltaTime();


	// Clear color buffer
	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	/* Render background quad */
	glDisable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);

	glUseProgram(uiProgram[E_LOOKUP]);
	glBindTexture(GL_TEXTURE_2D, uiCheckerTexture);

	TTFont::glEnable2D();
	RenderQuad(0, 0, ShellGet(SHELL_WIDTH), ShellGet(SHELL_HEIGHT), 0,0,1,1);
	TTFont::glDisable2D();


	glEnable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);

	// Select and setup the modelview matrix
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	gluLookAt( 0.0f, 1.0f, 0.0f,    // Eye-position
		       0.0f, 20.0f, 0.0f,   // View-point
		       0.0f, 0.0f, 1.0f );  // Up-vector

	// Draw a rotating colorful triangle
	glTranslatef( 0.0f, 14.0f, 0.0f );
	glRotatef( x + (GLfloat)t*30.0f, 0.0f, 0.0f, 1.0f );
	glRotatef( y, 0.0f, 1.0f, 0.0f );
	
	/* Draw triangle */
	glUseProgram(uiProgram[E_COLOR]);

	glEnableClientState(GL_VERTEX_ARRAY);	
	glEnableClientState(GL_COLOR_ARRAY);
	pTriangleVBO->Render();
	glDisableClientState(GL_VERTEX_ARRAY);	
	glDisableClientState(GL_COLOR_ARRAY);


	/* Draw mesh */
	glUseProgram(uiProgram[E_UNIFORM]);

	glEnableClientState(GL_VERTEX_ARRAY);	
	pMonkeyVBO->Render();
	glDisableClientState(GL_VERTEX_ARRAY);	

	/* Draw font */
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	//glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR);
	glEnable(GL_TEXTURE_2D);

	glUseProgram(uiProgram[E_LOOKUP]);

	TTFont::glEnable2D();
	
	SDL_Color color;
	SDL_Rect position;

	color.r = 255;
	color.g = 255;
	color.b = 255;

	position.x = 0;
	position.y = ShellGet(SHELL_HEIGHT) - 40;

	char text[40];
	sprintf(text, "%.1f",  1.0f / timer.GetDeltaTime());
	ttFont.SDL_GL_RenderText(text, color, &position);
	position.y -= position.h;

	TTFont::glDisable2D();
	
	return true;
}

bool Shadows::ReleaseGL()
{
	delete pTriangleVBO;
	delete pMonkeyVBO;

	return GLResourceManager::Instance().Release();
}

bool Shadows::ReleaseApp()
{
	return true;
}

Shell *Run()
{
	return new Shadows();
}

