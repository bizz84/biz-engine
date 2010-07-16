/*****************************************************************************
 * Filename			BigHeadScreamers.h
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
#include "FBO.h"
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
		E_LOOKUP,		// used for font
		E_LOOKUP_COLOR, // used for grenades
		E_COLOR_OFFSET, // used for coordinate frame
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
	
	FBO *pReflectionFBO;

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
	
	void Input();
	
	void SkyBoxRotate() const ;

	void DrawCoordinateFrame() const;
	void RenderGrenades() const;	
	void RenderSkyBox() const;
	void RenderReflection() const;
	void RenderGround();
	void ShowInfo();

	void RenderReflectionFBO();
	
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
	~BigHeadScreamers();
};

#endif

