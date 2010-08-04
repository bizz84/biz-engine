/*****************************************************************************
 * Filename			Shadows.h
 * 
 * License			GPLv3
 *
 * Author			Andrea Bizzotto (bizz84@gmail.com)
 *
 * Platform			LinuxX11 / OpenGL
 * 
 * Description		Lighting & Shadows demo using stencil buffers
 *					This sample implementation uses the Z-fail algorithm
 *					(aka Carmack's reverse)
 *
 *****************************************************************************/
#ifndef _SHADOWS_H_
#define _SHADOWS_H_

#include "Extensions.h"
#include "SDLShell.h"
#include "Misc.h"
#include "Timer.h"
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

#include "CameraController.h"

class TextGraph;
class FontManager;

#define GROUPS_COUNT 3
#define MAX_LIGHTS 4

enum Shaders {
	E_LOOKUP, 
	E_COLOR_OFFSET, 
	E_DIFFUSE, 
	E_UNIFORM, 
	E_SHADOW_VOLUME, 
	E_SHADOW,
	E_PLANE,
	E_NUM_PROGRAMS 
};

const char *shaders[] = {
	"data/shaders/Lookup.vert", "data/shaders/Lookup.frag",
	"data/shaders/ColorOffset.vert", "data/shaders/ColorOffset.frag",
	"data/shaders/DiffuseColor.vert", "data/shaders/DiffuseColor.frag",
	"data/shaders/UniformColor.vert", "data/shaders/UniformColor.frag",
	"data/shaders/ShadowVolume.vert", "data/shaders/ShadowVolume.frag",
	"data/shaders/Shadow.vert", "data/shaders/Shadow.frag",
	"data/shaders/Plane.vert", "data/shaders/UniformColor.frag",
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

/*class RollingSpinCamera : public SpinCamera
{
public:
	void Update(SDLShell *shell, float d, float a, float b)
	{
		alpha = a;
		beta = b;
		SpinCamera::Update(shell, d);
	}
};*/

class Shadows : public SDLShell
{

protected:
	GLuint uiProgram[E_NUM_PROGRAMS];

	int iShowInfo;

	float y, x, t;
	float xRot, yRot;
	bool bWireframe;

	Timer timer;

	GLuint uiBGTexture;
	GLuint uiCoordFrameList;

	float fMaxDistance, fMinDistance;
	float fDefDistance;

	float fBoxSize;
	float fDistance;

	float fShadowExtent;
	float fMinShadowExtent;
	float fMaxShadowExtent;

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

	bool bUseDoubleStencil;

	bool bFPSMode;

	int iFrameCounter;
	int iCurrentGroup;

	IndexedVBO *pTetraVBO;
	ShadowVolumeMesh *pSVMesh[MESH_NUM];
	Mesh *pRoomMesh;

	std::vector<int> aaiMeshGroup[GROUPS_COUNT];

	int eDisplayMode;

	auto_ptr<TextGraph> pFPSGraph;
	auto_ptr<FontManager> pFont;

	FPSCamera fpsCamera;
	//RollingSpinCamera spinCamera;

	bool LoadShaders();

	void UpdateModelView();

	void DrawLightMarker(float *lightPos);
	void DrawCoordinateFrame();
	void CalculateBoundaries(int index3ds);

	void PreRenderGeometry(GLuint program);
	void PostRenderGeometry(GLuint program);

	void RenderGeometry(GLuint program);
	void RenderShadowVolumes(unsigned int lightIndex);
	void RenderScene();
	void ShowShadowVolume(unsigned int lightIndex);
	void RenderCurrentGroup(bool shadowVolume);

	void CurrentGroupDesc(char *str);

	void RenderPlane(GLuint program, const float *color);

	GLenum GLRenderMode() { return bWireframe ? GL_LINES : GL_TRIANGLES; }

	// Virtuals
	virtual bool InitApp();
	virtual bool InitGL();
	virtual bool Render();
	virtual bool ReleaseGL();
	virtual bool ReleaseApp();	
	virtual bool Resize(unsigned int width, unsigned int height);

	virtual bool RequiresOpenGL2() { return true; }
	virtual bool RequiresTTF()  { return true; }

	virtual const char *GetAppName() const { return szAppName; }
	virtual const char *GetAppVersion() const { return szAppVersion; }

public:
	Shadows();
};

#endif