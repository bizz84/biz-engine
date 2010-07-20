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
#include "AIManager.h"
#include "WeaponRenderer.h"
#include "CollisionDetector.h"

class BigHeadScreamers : public SDLShell
{
	enum EShaders {
		E_LOOKUP,		// used for font
		E_SPRITE,		// used for sprites
		E_LOOKUP_COLOR, // used for grenades
		E_COLOR_OFFSET, // used for coordinate frame
		NUM_PROGRAMS 
	};

	enum { NUM_TEXTURES = 2 };
	enum { NUM_CUBEMAPS = 5 };


	Pointer *NewPointer() { return new FPSPointer(this); }
protected:

	int iShowInfo;

	// Ground texture data
	unsigned int uiCurTexture;
	GLuint uiTexture[NUM_TEXTURES];
	const GLuint CurrentTexture() const { return uiTexture[uiCurTexture]; }
	void NextTexture() { uiCurTexture = Next(uiCurTexture, NUM_TEXTURES); }
	void PrevTexture() { uiCurTexture = Prev(uiCurTexture, NUM_TEXTURES); }

	// Cubemaps data
	unsigned int uiCurCubemap;
	CubeMap cubemap[NUM_CUBEMAPS];
	const CubeMap &CurrentCubemap() const { return cubemap[uiCurCubemap]; }
	void NextCubemap() { uiCurCubemap = Next(uiCurCubemap, NUM_CUBEMAPS); }
	void PrevCubemap() { uiCurCubemap = Prev(uiCurCubemap, NUM_CUBEMAPS); }

	
	FBO *pReflectionFBO;
	
	// mutable since it's changed by RenderReflection() but restored at the end
	// aka logical constness
	mutable bool bReflectionFlag;

	// Shader program files
	GLuint uiProgram[NUM_PROGRAMS];

	// Timing related variables
	Timer timer;
	float fSetTime;
	float fRandomTime;

	TTFont ttFont;


	// Inverted projection matrix (needed by infinite plane rendering)
	Matrix4 mInvProj;
	Ground ground;

	BaseGraph fpsGraph;
	FPSCamera fpsCamera;

	// Game Data
	WeaponSystem *pWS;
	WeaponRenderer *pWR;
	AIManager *pAIManager;

	CollisionDetector *pDetector;

protected:
	// Resource loading
	bool LoadShaders();
	bool LoadTextures();
	bool LoadCubemaps();

	
	// All input is processed here
	void Input();

	// Simple utility functions
	inline unsigned int Next(unsigned int n, unsigned int mod)
	{
		return n < mod - 1 ? n + 1 : 0;
	}
	inline unsigned int Prev(unsigned int n, unsigned int mod)
	{
		return n > 0 ? n - 1 : mod - 1;
	}
	
	// Auxiliary methods to Render functions
	void SkyBoxRotate() const;
	void MultMirror() const;

	// Rendering methods
	void RenderReflection() const;
	void RenderScene() const;
	void RenderSkyBox() const;
	void RenderGround() const;
	void RenderFire() const;	
	void RenderSprites() const;
	void DrawCoordinateFrame() const;
	void RenderReflectionFBO();
	void ShowInfo();
	
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

