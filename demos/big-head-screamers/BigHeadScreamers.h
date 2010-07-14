/*****************************************************************************
 * Filename			BigHeadScreamers.cpp
 * 
 * License			LGPL
 *
 * Author			Andrea Bizzotto (bizz84@gmail.com)
 *
 * Platform			LinuxX11 / OpenGL
 * 
 * Description		Lighting & BigHeadScreamers demo using stencil buffers
 *
 *****************************************************************************/

#ifndef _BIG_HEAD_SCREAMERS_
#define _BIG_HEAD_SCREAMERS_


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
#include "Mesh.h"
#include "ShadowVolume.h"

#include "version.h"


#include "Matrix.h"
#include "CameraController.h"
#include "Weapon.h"

#include "SkyBox.h"


#define GROUPS_COUNT 3
#define MAX_LIGHTS 4

enum Shaders {
	E_LOOKUP, 
	E_LOOKUP_COLOR, 
	E_INFINITY, 
	E_IDENTITY, 
	E_DIFFUSE, 
	E_COLOR_OFFSET,
	E_UNIFORM,
	E_NUM_PROGRAMS 
};

const char *shaders[] = {
	"data/shaders/Lookup.vert", "data/shaders/Lookup.frag",
	"data/shaders/LookupColor.vert", "data/shaders/LookupColor.frag",
	"data/shaders/Infinite.vert", "data/shaders/Infinite.frag",
	"data/shaders/Identity.vert", "data/shaders/Identity.frag",
	"data/shaders/DiffuseColor.vert", "data/shaders/DiffuseColor.frag",
	"data/shaders/ColorOffset.vert", "data/shaders/ColorOffset.frag",
	"data/shaders/UniformColor.vert", "data/shaders/UniformColor.frag",
};


enum Primitives {
	MESH_TORUS_KNOT, 
	MESH_TORUS, 
	MESH_SPHERE,
	MESH_CUBE,
	MESH_MONKEY, 
	MESH_NUM 
};

const char *meshes[] = {
	"TorusKnot",
	"Torus",
	"Sphere",
	"Cube",
	"Suzanne",
};




class BigHeadScreamers : public SDLShell
{
	Pointer *NewPointer() { return new FPSPointer(this); }
protected:
	GLuint uiProgram[E_NUM_PROGRAMS];

	CubeMap alpinCubeMap;

	int iShowInfo;

	bool bWireframe;

	Timer timer;

	TTFont ttFont;

	GLuint uiBGTexture;
	GLuint uiCoordFrameList;

	float fMaxDistance, fMinDistance;
	float fDefDistance;

	float fBoxSize;
	float fDistance;

	// Temp
	unsigned int uiInfPlaneVertices;
	Vector3 vGround[5];


	// Light defines a light structure containing all variables used for
	// animation, and the position to be passed to the API
	struct Light
	{
		float fLightSpeed;
		float fLightAngle;
		float fLightHeight;
		float fLightRadius;
		//! Position to be passed to glLightfv
		float fPos[4];

		void UpdatePos(float minDistance)
		{
			fPos[0] = fLightRadius * minDistance * 
				cos(fLightAngle * fLightSpeed);
			fPos[1] = fLightHeight;
			fPos[2] = fLightRadius * minDistance *
				sin(fLightAngle * fLightSpeed);
			fPos[3] = 1.0f;
		}

	} light[MAX_LIGHTS];
	unsigned int iCurLight;
	unsigned int iNumLights;

	bool bLightMove;


	IndexedVBO *pTetraVBO;
	Mesh *pRoomMesh;

	BaseGraph fpsGraph;

	FPSCamera fpsCamera;

	bool LoadShaders();

	void DrawLightMarker(float *lightPos);
	void DrawCoordinateFrame();
	void CalculateBoundaries(int index3ds);

	void RenderRoom(GLuint program);
	void RenderScene();

	void RenderPlane(GLuint program, const float *color);

	GLenum GLRenderMode() { return bWireframe ? GL_LINES : GL_TRIANGLES; }

	void Projection(Vector4 &u, const float *proj, const float *mview, const Vector4 &v);
	void ZeroZ(Vector4 &w, const Vector4 &u);

	// Game Data
	GrenadeLauncher launcher;

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

