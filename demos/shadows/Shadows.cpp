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

#include <math.h>
#include <stdio.h>

#include "GLResourceManager.h"

enum Shaders { E_LOOKUP, E_COLOR };

class Shadows : public SDLShell
{

protected:
	GLuint uiProgram[2];

	float x, t;
	Timer timer;

	TTFont ttFont;

	GLuint uiCheckerTexture;

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
{
	x = t = 0.0f;
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

	if (!ttFont.LoadFont("data/fonts/LucidaBrightDemiBold.ttf", 20))
		return false;
	

	if (!loader.LoadTextureFromFile("data/textures/perspec_warp_input_tex.bmp", uiCheckerTexture, GL_LINEAR, GL_LINEAR))
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
	timer.Update();

	x += timer.GetDeltaTime();
	t += timer.GetDeltaTime();


	// Clear color buffer
	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	glDisable(GL_BLEND);

	glEnable(GL_TEXTURE_2D);

	glUseProgram(uiProgram[E_LOOKUP]);

	glBindTexture(GL_TEXTURE_2D, uiCheckerTexture);

	TTFont::glEnable2D();

	RenderQuad(0, 0, ShellGet(SHELL_WIDTH), ShellGet(SHELL_HEIGHT), 0,0,1,1);

	TTFont::glDisable2D();



	glDisable(GL_TEXTURE_2D);

	// Select and setup the modelview matrix
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	gluLookAt( 0.0f, 1.0f, 0.0f,    // Eye-position
		       0.0f, 20.0f, 0.0f,   // View-point
		       0.0f, 0.0f, 1.0f );  // Up-vector

	// Draw a rotating colorful triangle
	glTranslatef( 0.0f, 14.0f, 0.0f );
	glRotatef( 0.3*(GLfloat)x + (GLfloat)t*100.0f, 0.0f, 0.0f, 1.0f );
	

	glUseProgram(uiProgram[E_COLOR]);

	glEnableClientState(GL_VERTEX_ARRAY);	
	glEnableClientState(GL_COLOR_ARRAY);
	float afAttribs[] = {
	  -5.0f, 0.0f, -4.0f, 1.0f, 0.0f, 0.0f,
	  5.0f, 0.0f, -4.0f, 0.0f, 1.0f, 0.0f,
	  0.0f, 0.0f, 6.0f, 0.0f, 0.0f, 1.0f
	};
	glVertexPointer(3, GL_FLOAT, sizeof(float) * 6, afAttribs);
	glColorPointer(3, GL_FLOAT, sizeof(float) * 6, afAttribs + 3);

	glDrawArrays(GL_TRIANGLES, 0, 3);

	glDisableClientState(GL_VERTEX_ARRAY);	
	glDisableClientState(GL_COLOR_ARRAY);


	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	//glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR);
	glEnable(GL_TEXTURE_2D);




	glUseProgram(uiProgram[E_LOOKUP]);

	TTFont::glEnable2D();

	glDisable(GL_DEPTH_TEST);
	
	SDL_Color color;
	SDL_Rect position;

	color.r = 255;
	color.g = 255;
	color.b = 255;

	position.x = ShellGet(SHELL_WIDTH) / 3;
	position.y = ShellGet(SHELL_HEIGHT) / 2;
	ttFont.SDL_GL_RenderText("Hello, World!", color, &position);
	position.y -= position.h;
	ttFont.SDL_GL_RenderText("A line right underneath", color, &position);
	position.y -= position.h;
	ttFont.SDL_GL_RenderText("Yay text rendering.", color, &position);

	char fps[20];
	sprintf(fps, "%.3f",  1.0f / timer.GetDeltaTime());
	position.y -= position.h;
	ttFont.SDL_GL_RenderText(fps, color, &position);

	TTFont::glDisable2D();
	
	return true;
}

bool Shadows::ReleaseGL()
{
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
