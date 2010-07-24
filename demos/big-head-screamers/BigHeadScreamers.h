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

#ifndef _BIG_HEAD_SCREAMERS_H_
#define _BIG_HEAD_SCREAMERS_H_

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
#include "Matrix.h"
#include "CoordinateFrame.h"

#include "BaseGraph.h"

#include "CameraController.h"
#include "SkyBox.h"
#include "Ground.h"

class WeaponSystem;
class WeaponRenderer;
class AIManager;
class EnemyRenderer;
class CollisionDetector;

// is-a SDLShell, is-implemented-in-terms-of ProgramArray
class BigHeadScreamers : public SDLShell, private ProgramArray
{
	enum {
		P_LOOKUP,		// used for font
		P_COLOR_OFFSET, // used for coordinate frame
		NUM_PROGRAMS 
	};

	enum { NUM_TEXTURES = 2 };
	enum { NUM_CUBEMAPS = 5 };

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

	// Timing related variables
	Timer timer;
	float fSetTime;
	float fRandomTime;

	TTFont ttFont;


	// Inverted projection matrix (needed by infinite plane rendering)
	Matrix4 mInvProj;
	Ground ground;

	BaseGraph fpsGraph;
	BaseGraph mouseGraph[2];
	FPSCamera fpsCamera;

	// Game Data
	WeaponSystem *pWS;
	WeaponRenderer *pWR;
	AIManager *pAI;
	// Two versions loaded for benchmarking
	int iCurER;
	EnemyRenderer *pER[2];

	CollisionDetector *pDetector;

	float fCollisionTime;
	// This is updated during render time
	float fEnemiesTime;

	// Overrides SDLShell version
	virtual Pointer *NewPointer() { return new FPSPointer(this); }
protected:
	// Resource loading
	bool LoadTextures();
	bool LoadCubemaps();

	
	// All input is processed here (called by Render())
	// TODO implement into SDLShell as non-const, make Render() const
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
	void RenderReflectionFBO() const;
	void ShowInfo(); // TODO const
	
	// Virtuals
	virtual bool InitApp();
	virtual bool InitGL();
	virtual bool Render(); // TODO: const once Input is moved out
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

