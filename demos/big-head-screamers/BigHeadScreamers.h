/*****************************************************************************
 * Filename			BigHeadScreamers.cpp
 * 
 * License			LGPL
 *
 * Author			Andrea Bizzotto (bizz84@gmail.com)
 *
 * Platform			LinuxX11 / OpenGL
 * 
 * Description		BigHeadScreamers
 *
 *****************************************************************************/

#ifndef _BIG_HEAD_SCREAMERS_
#define _BIG_HEAD_SCREAMERS_

#include "version.h"

#include "Extensions.h"
#include "SDLShell.h"
#include "Misc.h"
#include "Timer.h"
#include "TTFont.h"
#include "Keys.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>

#include "GLResourceManager.h"
#include "VBO.h"
#include "CoordinateFrame.h"

#include "BaseGraph.h"

#include "Matrix.h"
#include "CameraController.h"
#include "Weapon.h"
#include "SkyBox.h"
#include "Ground.h"

class BigHeadScreamers : public SDLShell
{
	enum Shaders {
		E_LOOKUP, 
		E_LOOKUP_COLOR, 
		E_COLOR_OFFSET,
		E_UNIFORM,
		E_NUM_PROGRAMS 
	};

	Pointer *NewPointer() { return new FPSPointer(this); }
protected:
	// Game Data
	GrenadeLauncher launcher;

	// POD part of the file
	int iShowInfo;
	GLuint uiBGTexture;

	IndexedVBO *pTetraVBO;

	// Shader program files
	GLuint uiProgram[E_NUM_PROGRAMS];

	Timer timer;

	TTFont ttFont;

	CubeMap alpinCubeMap;
	// Inverted projection matrix (needed by infinite plane rendering)
	Matrix4 mInvProj;
	Ground ground;

	BaseGraph fpsGraph;
	FPSCamera fpsCamera;

protected:
	bool LoadShaders();

	void DrawLightMarker(float *lightPos);
	void DrawCoordinateFrame();

	void RenderGrenades();

	// Virtuals
	virtual bool InitApp();
	virtual bool InitGL();
	virtual bool Render();
	virtual bool ReleaseGL();
	virtual bool ReleaseApp();	
	virtual bool Resize(unsigned int width, unsigned int height);

	virtual bool RequiresOpenGL2() { return true; }
	virtual bool RequiresTTF()  { return true; }

	virtual const char *GetAppName() { return szAppName; }
	virtual const char *GetAppVersion()  { return szAppVersion; }

public:
	BigHeadScreamers();
};

#endif

