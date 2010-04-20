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
 *					This sample implementation uses the Z-fail algorithm
 *					(aka Carmack's reverse)
 *
 *****************************************************************************/

#include "Extensions.h"
#include "SDLShell.h"
#include "Misc.h"
#include "Timer.h"
#include "TTFont.h"
#include "Keys.h"

#include <math.h>
#include <stdio.h>

#include "GLResourceManager.h"
#include "VBO.h"
#include "CoordinateFrame.h"

#include "BaseGraph.h"
#include "Mesh.h"
#include "ShadowVolume.h"

#include "version.h"

enum DisplayMode {
	E_SHADOWS,
	E_SHADOW_VOLUMES,
	E_STENCIL,
	//E_NO_SHADOWS, 
	E_DISPLAY_MODES 
};

const char *DisplayMoreStr[] = {
	"Shadow Volumes",
	"Shadows",
	"Stencil Count",
	"No Shadows"
};

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

const float TetraVertices[12] = {
	 0.0f    ,  1.0f    ,  0.0     , 
	 0.0f    , -0.33333f,  0.94281f,
	-0.81650f, -0.33333f, -0.47140f,
	 0.81650f, -0.33333f, -0.47140f
};

const int TetraIndices[12] = {
	0, 1, 2,
	0, 2, 3,
	0, 3, 1,
	1, 2, 3
};

const float Colors[] = {

	0.0f, 1.0f, 0.0f, 0.8f,
	0.0f, 1.0f, 0.0f, 0.6f,
	0.0f, 1.0f, 0.0f, 0.4f,
	0.0f, 1.0f, 0.0f, 0.2f,
	1.0f, 1.0f, 0.0f, 0.2f,
};

const float GrayColor[] = { 0.5f, 0.5f, 0.5f, 1.0f };
const float WhiteColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };

#define GROUPS_COUNT 3

class Shadows : public SDLShell
{

protected:
	GLuint uiProgram[E_NUM_PROGRAMS];

	int iShowInfo;

	float y, x, t;
	bool bWireframe;

	Timer timer;

	TTFont ttFont;

	GLuint uiBGTexture;
	GLuint uiCoordFrameList;

	float fMaxDistance, fMinDistance;
	float fDefDistance;

	float fBoxSize;
	float fDistance;

	float fShadowExtent;
	float fMinShadowExtent;
	float fMaxShadowExtent;

	//bool bInfiniteShadowVolume;
	// Light related members
	float fLightSpeed;
	float fLightAngle;
	float fLightHeight;
	float fLightRadius;
	bool bLightMove;

	bool bUseDoubleStencil;

	int iFrameCounter;
	int iCurrentGroup;

	IndexedVBO *pTetraVBO;
	ShadowVolumeMesh *pSVMesh[MESH_NUM];
	Mesh *pRoomMesh;

	std::vector<int> aaiMeshGroup[GROUPS_COUNT];

	int eDisplayMode;

	BaseGraph fpsGraph;

	bool LoadShaders();

	void DrawLightMarker(float xRot, float yRot, float *lightPos);
	void DrawCoordinateFrame(float xRot, float yRot);
	void CalculateBoundaries(int index3ds);

	void PreRenderGeometry(GLuint program);
	void PostRenderGeometry(GLuint program);

	void RenderGeometry(GLuint program);
	void RenderShadowVolumes();
	void RenderScene(float *light);
	void ShowShadowVolume();
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

	virtual const char *GetAppName() { return szAppName; }
	virtual const char *GetAppVersion()  { return szAppVersion; }

public:
	Shadows();
};

Shadows::Shadows()
{
	t = x = y = 0.0f;
	bWireframe = false;
	unsigned int i;
	for (i = 0; i < MESH_NUM; i++)
	{
		pSVMesh[i] = NULL;
	}
	pTetraVBO = NULL;
	pRoomMesh = NULL;

	eDisplayMode = E_SHADOWS;

	//bInfiniteShadowVolume = false;

	iFrameCounter = 0;

	iCurrentGroup = 0;

	fLightSpeed = 0.5f;
	fLightAngle = 0.0f;
	fLightHeight = 100.0f;
	fLightRadius = 1.0f;
	bLightMove = true;

	bUseDoubleStencil = true;
}

bool Shadows::InitApp()
{
	iShowInfo = 0;

	// Custom command line processing
	vector<CmdLineParameter>::iterator iter;
	vector<CmdLineParameter> &params = GetCommandLineParams();

	for (iter = params.begin(); iter != params.end(); iter++)
	{
		if (iter->sName == "showinfo")
		{
			iShowInfo = atoi(iter->sValue.c_str());
		}
	}	

	return true;
}

bool Shadows::LoadShaders()
{
	GLResourceManager &loader = GLResourceManager::Instance();
	for (unsigned int i = 0; i < E_NUM_PROGRAMS; i++)
	{
		if (!loader.LoadShaderFromFile(shaders[i*2+0], shaders[i*2+1], uiProgram[i]))
			return false;
	}
	return true;
}

bool Shadows::InitGL()
{
	GLResourceManager &loader = GLResourceManager::Instance();

	// Load Shaders
	if (!LoadShaders())
		return false;

	// Load font used for overlay rendering
	if (!ttFont.LoadFont("data/fonts/LucidaBrightDemiBold.ttf", 20))
		return false;	

	if (!loader.LoadTextureFromFile("data/textures/crate-base.bmp", uiBGTexture, GL_LINEAR, GL_LINEAR))
		return false;
	
	unsigned int handle3DSFile[2];
	if (!loader.Load3DSFile("data/scenes/primitives.3ds", handle3DSFile[0]))
		return false;

	if (!loader.Load3DSFile("data/scenes/sphere-cube.3ds", handle3DSFile[1]))
		return false;

	//if (!loader.Load3DSFile("data/scenes/monkey.3ds", handle3DSFile[2]))
	//	return false;

	// Load outer room
	pRoomMesh = new Mesh(loader.FindMesh(handle3DSFile[0], "Box"));
	
	// Load Object Meshes and generate Shadow Volumes
	pSVMesh[MESH_TORUS] = new ShadowVolumeMesh(loader.FindMesh(handle3DSFile[0], meshes[MESH_TORUS]));
	pSVMesh[MESH_TORUS_KNOT] = new ShadowVolumeMesh(loader.FindMesh(handle3DSFile[0], meshes[MESH_TORUS_KNOT]));
	
	pSVMesh[MESH_CUBE] = new ShadowVolumeMesh(loader.FindMesh(handle3DSFile[1], meshes[MESH_CUBE]));
	pSVMesh[MESH_SPHERE] = new ShadowVolumeMesh(loader.FindMesh(handle3DSFile[1], meshes[MESH_SPHERE]));

	//pSVMesh[MESH_MONKEY] = new ShadowVolumeMesh(loader.FindMesh(handle3DSFile[2], meshes[MESH_MONKEY]), 60.0f);


	aaiMeshGroup[0].push_back(MESH_TORUS_KNOT);
	aaiMeshGroup[1].push_back(MESH_TORUS);
	aaiMeshGroup[2].push_back(MESH_SPHERE);
	aaiMeshGroup[2].push_back(MESH_CUBE);


	CalculateBoundaries(handle3DSFile[0]);

	fMinShadowExtent = 0.0f;
	fMaxShadowExtent = 2.0f * fMaxDistance;
	fShadowExtent = fMaxShadowExtent;

	pTetraVBO = new IndexedVBO((void *)TetraVertices, sizeof(float) * 3, 4, (void *)TetraIndices, 12);


	if (!fpsGraph.Init(4.0f, 3000, -0.98f, 0.60f, -0.6f, 0.95f, false))
	{
		return false;
	}


	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );

	Resize(ShellGet(SHELL_WIDTH), ShellGet(SHELL_HEIGHT));

	glDisable(GL_DEPTH_TEST);

	// Hack
	fMaxDistance *= 3.0f;

	timer.Start();
	return true;
}

void Shadows::CalculateBoundaries(int index3ds)
{
	GLResourceManager &loader = GLResourceManager::Instance();

	// Calculating maximum and minimum range for zoom
	fBoxSize = loader.MeshSize(index3ds, "Box");

	fMaxDistance = 0.5f * fBoxSize;
	
	float d = fMinDistance = loader.MeshSize(index3ds, "Torus");
	d = loader.MeshSize(index3ds, "TorusKnot");
	if (d > fMinDistance)
		fMinDistance = d;
	d = loader.MeshSize(index3ds, "Teapot");
	if (d > fMinDistance)
		fMinDistance = d;

	fDistance = fDefDistance = 0.5f * (fMaxDistance + fMinDistance);

	CoordinateFrame::Instance().Make(fMaxDistance);
}

bool Shadows::Resize(unsigned int width, unsigned int height)
{
	//printf("Reshaping Screen to %dx%d\n", width, height);
	// Select and setup the projection matrix
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluPerspective( 65.0f, (GLfloat)width/(GLfloat)height, 10.0f, 4000.0f );
	return true;
}

bool Shadows::Render()
{
	/* Input - timer and fps graph*/
	timer.Update();

	fpsGraph.Input(1.0f / timer.GetDeltaTime(), timer.GetTime());

	/* User input */
	if (KeyPressed(KEY_SPACE))
	{
		iCurrentGroup = (iCurrentGroup + 1) % (int)GROUPS_COUNT;
	}
	if (KeyPressed(KEY_ENTER))
	{
		eDisplayMode = (eDisplayMode + 1) % E_DISPLAY_MODES;
	}
	if (KeyPressing(KEY_LEFT))
	{
		if ((fShadowExtent -= timer.GetDeltaTime() * 300.0f) < fMinShadowExtent)
			fShadowExtent = fMinShadowExtent;
	}
	if (KeyPressing(KEY_RIGHT))
	{
		if ((fShadowExtent += timer.GetDeltaTime() * 300.0f) > fMaxShadowExtent)
			fShadowExtent = fMaxShadowExtent;
	}
	//if (KeyPressed(KEY_7))
	//{
	//	bInfiniteShadowVolume = !bInfiniteShadowVolume;
	//}
	if (KeyPressed(KEY_8))
	{
		bUseDoubleStencil = !bUseDoubleStencil;
	}
	if (KeyPressed(KEY_9))
	{
		bLightMove = !bLightMove;
	}
	if (KeyPressed(KEY_0))
	{
		bWireframe = !bWireframe;
	}
	if (KeyPressing(KEY_UP))
	{
		if ((fDistance -= timer.GetDeltaTime() * 150.0f) < fMinDistance)
			fDistance = fMinDistance;
	}
	if (KeyPressing(KEY_DOWN))
	{
		if ((fDistance += timer.GetDeltaTime() * 150.0f) > fMaxDistance)
			fDistance = fMaxDistance;
	}
	if (ScrollUp())
	{
		if ((fDistance /= 1.05f) < fMinDistance)
			fDistance = fMinDistance;
	}
	if (ScrollDown())
	{
		if ((fDistance *= 1.05f) > fMaxDistance)
			fDistance = fMaxDistance;
	}

	if (KeyPressing(KEY_Q))
	{
		fLightHeight += 50.0f * timer.GetDeltaTime();
	}
	if (KeyPressing(KEY_E))
	{
		fLightHeight -= 50.0f * timer.GetDeltaTime();
	}
	if (KeyPressing(KEY_W))
	{
		fLightRadius += timer.GetDeltaTime();
	}
	if (KeyPressing(KEY_S))
	{
		fLightRadius -= timer.GetDeltaTime();
	}

	if (bLightMove)
	{
		fLightAngle += timer.GetDeltaTime();
	}

	/* Update camera state */
	y += 0.002f * (GetPointer()->GetMotionX() + GetPointer()->GetInertiaX());
	x -= 0.002f * (GetPointer()->GetMotionY() + GetPointer()->GetInertiaY());
	if (GetPointer()->TimeSinceLastInput() > 10.0f && bLightMove)
		t += timer.GetDeltaTime();

	float yRot = y + 20.0f * t;
	float xRot = x + 20.0f * sin(t);

	/* Rendering */
	// Clear color and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);

	// Select and setup the modelview matrix
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	glTranslatef(0.0f, 0.0f, -fDistance);
	glRotatef(xRot, 1.0f, 0.0f, 0.0f);
	glRotatef(yRot, 0.0f, 1.0f, 0.0f);

	float light[4] = {
		fLightRadius * fMinDistance * cos(fLightAngle * fLightSpeed),
		fLightHeight,
		fLightRadius * fMinDistance * sin(fLightAngle * fLightSpeed),
		1.0f
	};

	glEnable(GL_CULL_FACE);

	// Draw scene (including shadow if enabled)
	RenderScene(light);

	glDisable(GL_CULL_FACE);

	// Render Light
	DrawLightMarker(xRot, yRot, light);

	//glDisable(GL_DEPTH_TEST);
	glClear(GL_DEPTH_BUFFER_BIT);

	DrawCoordinateFrame(xRot, yRot);

	if (iShowInfo)
	{
		fpsGraph.Draw();

		/* Draw font */
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
		glEnable(GL_TEXTURE_2D);

		glUseProgram(uiProgram[E_LOOKUP]);

		TTFont::glEnable2D();
		
		SDL_Color color;
		SDL_Rect position;

		color.r = 255;
		color.g = 255;
		color.b = 255;

		position.x = (int)(ShellGet(SHELL_WIDTH) * 0.01f);
		position.y = (int)(ShellGet(SHELL_HEIGHT) * 0.75f);

		if (iShowInfo == 1)
		{
			char text[200];
			sprintf(text, "%.1f",  1.0f / timer.GetDeltaTime());
			ttFont.SDL_GL_RenderText(text, color, &position);
		}
		else if (iShowInfo == 2)
		{
			char text[200];
			sprintf(text, "FPS=%.1f",  1.0f / timer.GetDeltaTime());
			ttFont.SDL_GL_RenderText(text, color, &position);
			position.y -= position.h * 1.2f;

			sprintf(text, "[LEFT,RIGHT] Extent=%.1f", fShadowExtent);
			ttFont.SDL_GL_RenderText(text, color, &position);
			position.y -= position.h * 1.2f;

			CurrentGroupDesc(text);
			ttFont.SDL_GL_RenderText(text, color, &position);
			position.y -= position.h * 1.2f;			

			sprintf(text, "[ENTER] Render Mode=%s", DisplayMoreStr[eDisplayMode]);
			ttFont.SDL_GL_RenderText(text, color, &position);
			position.y -= position.h * 1.2f;

			//sprintf(text, "[7] Infinite Shadow Volume=%d", bInfiniteShadowVolume ? 1 : 0);
			//ttFont.SDL_GL_RenderText(text, color, &position);
			//position.y -= position.h * 1.2f;

			sprintf(text, "[8] Double Stencil=%d", bUseDoubleStencil ? 1 : 0);
			ttFont.SDL_GL_RenderText(text, color, &position);
			position.y -= position.h * 1.2f;
			
			sprintf(text, "[0] Wireframe=%d", bWireframe ? 1 : 0);
			ttFont.SDL_GL_RenderText(text, color, &position);
			position.y -= position.h * 1.2f;
		}

		TTFont::glDisable2D();
		glDisable(GL_BLEND);
	}
	iFrameCounter++;
	
	return true;
}

void Shadows::PreRenderGeometry(GLuint program)
{
	/* Draw mesh */
	glUseProgram(program);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, uiBGTexture);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glCullFace(GL_BACK);
}

void Shadows::PostRenderGeometry(GLuint program)
{
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void Shadows::RenderGeometry(GLuint program)
{
	PreRenderGeometry(program);

	glUniform4fv(GetUniLoc(program, "Color"), 1, GrayColor);

	pRoomMesh->GetVBO()->Render(GL_TRIANGLES);

	glUniform4fv(GetUniLoc(program, "Color"), 1, WhiteColor);
	RenderCurrentGroup(false);

	PostRenderGeometry(program);
}


void Shadows::RenderCurrentGroup(bool shadowVolume)
{
	vector<int>::iterator iter;
	// Pointer to member function cannot be used here since the return types
	// are different
	if (shadowVolume)
	{
		for (iter = aaiMeshGroup[iCurrentGroup].begin(); iter != aaiMeshGroup[iCurrentGroup].end(); iter++)
		{
			pSVMesh[*iter]->GetShadowVolumeVBO()->Render(bWireframe ? GL_LINES : GL_TRIANGLES);
		}
	}
	else
	{
		for (iter = aaiMeshGroup[iCurrentGroup].begin(); iter != aaiMeshGroup[iCurrentGroup].end(); iter++)
		{
			pSVMesh[*iter]->GetVBO()->Render(bWireframe ? GL_LINES : GL_TRIANGLES);
		}
	}
}

void Shadows::CurrentGroupDesc(char *str)
{
	unsigned int VBOSize = 0;
	unsigned int shadowVBOSize = 0;
	char local[100];
	strcpy(local, "[SPACE] ( ");
	vector<int>::iterator iter;
	for (iter = aaiMeshGroup[iCurrentGroup].begin(); iter != aaiMeshGroup[iCurrentGroup].end(); iter++)
	{
		strcat(local, meshes[*iter]);
		strcat(local, " ");
		VBOSize += pSVMesh[*iter]->GetVBO()->GetCount();
		shadowVBOSize += pSVMesh[*iter]->GetShadowVolumeVBO()->GetCount();
	}
	strcat(local, ")");
	sprintf(str, "%s = (%d, %d)", local, VBOSize, shadowVBOSize);
}

void Shadows::ShowShadowVolume()
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glUseProgram(uiProgram[E_SHADOW_VOLUME]);

	glEnable(GL_BLEND);

	glUniform1f(GetUniLoc(uiProgram[E_SHADOW_VOLUME], "ShadowExtent"), fShadowExtent);
	glUniform1i(GetUniLoc(uiProgram[E_SHADOW_VOLUME], "InfiniteShadowVolume"), false/*bInfiniteShadowVolume*/);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	RenderCurrentGroup(true);

	glDisable(GL_BLEND);

	glDisableClientState(GL_NORMAL_ARRAY);	
	glDisableClientState(GL_VERTEX_ARRAY);
}

void Shadows::RenderShadowVolumes()
{
	// store current OpenGL state
	glPushAttrib(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_POLYGON_BIT | GL_STENCIL_BUFFER_BIT);

	GLuint program = uiProgram[E_SHADOW_VOLUME];
	glUseProgram(program);

	glUniform1f(GetUniLoc(program, "ShadowExtent"), fShadowExtent);
	glUniform1i(GetUniLoc(program, "InfiniteShadowVolume"), false/*bInfiniteShadowVolume*/);

	glClear(GL_STENCIL_BUFFER_BIT);

	if (eDisplayMode == E_SHADOW_VOLUMES)
		glEnable(GL_BLEND);
	else
		glColorMask(0, 0, 0, 0); // do not write to the color buffer

	glDepthMask(0); // do not write to the depth (Z) buffer
	glEnable(GL_STENCIL_TEST); // enable stencil testing

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	// set the reference stencil value to 0
	glStencilFunc(GL_ALWAYS, 0, ~0);

	if (bUseDoubleStencil)
	{
		glDisable(GL_CULL_FACE); // cull faces (back or front)

		glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
		glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);

		// Render group of meshes
		RenderCurrentGroup(true);
	}
	else
	{
		glEnable(GL_CULL_FACE); // cull faces (back or front)

		// Pass 1 for Z-fail
		// increment the stencil value on Z fail
		glStencilOp(GL_KEEP, GL_INCR, GL_KEEP);
		// draw only the back faces of the shadow volume
		glCullFace(GL_FRONT);

		// Render group of meshes
		RenderCurrentGroup(true);

		//glStencilFunc(GL_ALWAYS, 0, ~0);
		// Pass 2 for Z-fail
		// decrement the stencil value on Z fail
		glStencilOp(GL_KEEP, GL_DECR, GL_KEEP);
		// draw only the front faces of the shadow volume
		glCullFace(GL_BACK);

		// Render group of meshes
		RenderCurrentGroup(true);
	}



	glDisableClientState(GL_NORMAL_ARRAY);	
	glDisableClientState(GL_VERTEX_ARRAY);

	if (eDisplayMode == E_SHADOW_VOLUMES)
		glDisable(GL_BLEND);
	else
		glColorMask(1, 1, 1, 1); // do not write to the color buffer
	glDepthMask(1); // do not write to the depth (Z) buffer

	// restore OpenGL state
	glPopAttrib();
}

void Shadows::RenderPlane(GLuint program, const float *color)
{
	float vertexAttrib[] = {
		-1.0f, -1.0f,
		-1.0f, 1.0f,
		1.0f, -1.0f,
		1.0f, 1.0f,
	};

	glEnableClientState(GL_VERTEX_ARRAY);

	glVertexPointer(2, GL_FLOAT, 0, vertexAttrib);

	glUniform4fv(GetUniLoc(program, "Color"), 1, color);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glDisableClientState(GL_VERTEX_ARRAY);

}

void Shadows::RenderScene(float *light)
{
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_POSITION, light);

	glDepthFunc(GL_LESS);

	switch (eDisplayMode)
	{
	default:
	//case E_NO_SHADOWS:
	//	RenderGeometry(uiProgram[E_DIFFUSE]);
	//	break;
	//case E_SHADOW_VOLUMES:
	//	RenderGeometry(uiProgram[E_DIFFUSE]);
	//	ShowShadowVolume();
	//	break;
	case E_SHADOW_VOLUMES:
	case E_SHADOWS:

		/* First, render all geometry with color and depth write enabled */
		RenderGeometry(uiProgram[E_DIFFUSE]);

		/* Then, render the shadow volumes with color and depth write 
		   disabled. Use the Z-fail approach */
		RenderShadowVolumes();

		/* Finally, render again all the geometry setting a uniform
		   transparency factor affecting only non-zero stencil pixels
		   (areas in shadow) */
		glDepthFunc(GL_EQUAL);

		// Enable Stencil Test
		glEnable(GL_STENCIL_TEST);

		// update the color only where the stencil value is 0
		glStencilFunc(GL_NOTEQUAL, 0, ~0);
		
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

		glEnable(GL_BLEND);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//RenderGeometry(uiProgram[E_DIFFUSE]);
		RenderGeometry(uiProgram[E_SHADOW]);

		glDisable(GL_STENCIL_TEST);
		glDisable(GL_BLEND);
		glDepthFunc(GL_LESS);
		break;

	/* Start with same approach, then render multiple passes corresponding to
	   different values in the stencil buffer */
	case E_STENCIL:
		RenderGeometry(uiProgram[E_DIFFUSE]);

		RenderShadowVolumes();

		// re-draw the model with the light enabled only where
		// it has previously been drawn
		glDisable(GL_DEPTH_TEST);

		// update the color only where the stencil value is 0
		glEnable(GL_STENCIL_TEST);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);		
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

		glEnable(GL_BLEND);

		glDisable(GL_CULL_FACE);
		glUseProgram(uiProgram[E_PLANE]);
		for (unsigned int i = 1; i < 5; i++)
		{
			glStencilFunc(GL_EQUAL, i, ~0);

			RenderPlane(uiProgram[E_PLANE], Colors + (i-1) * 4);
		}
		glStencilFunc(GL_GEQUAL, 5, ~0);
		RenderPlane(uiProgram[E_PLANE], Colors + 16 * 4);

		glDisable(GL_STENCIL_TEST);
		glEnable(GL_CULL_FACE);
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		break;
	}
}


void Shadows::DrawLightMarker(float xRot, float yRot, float *lightPos)
{
	float white[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glUseProgram(uiProgram[E_UNIFORM]);
	glUniform4fv(GetUniLoc(uiProgram[E_UNIFORM], "Color"), 1, white);

	glLoadIdentity();

	glTranslatef(0.0f, 0.0f, -fDistance);
	glRotatef(xRot, 1.0f, 0.0f, 0.0f);
	glRotatef(yRot, 0.0f, 1.0f, 0.0f);

	glTranslatef(lightPos[0], lightPos[1], lightPos[2]);
	glScalef(10.0f, 10.0f, 10.0f);
	glEnableClientState(GL_VERTEX_ARRAY);
	pTetraVBO->Render(GL_TRIANGLES);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void Shadows::DrawCoordinateFrame(float xRot, float yRot)
{
	glUseProgram(uiProgram[E_COLOR_OFFSET]);
	float offset[] = { -0.9f, -0.9f };
	glUniform2fv(GetUniLoc(uiProgram[E_COLOR_OFFSET], "Offset"), 1, offset);

	glLoadIdentity();

	glTranslatef(0.0f, 0.0f, -fDefDistance);
	glRotatef(xRot, 1.0f, 0.0f, 0.0f);
	glRotatef(yRot, 0.0f, 1.0f, 0.0f);

	CoordinateFrame::Instance().Render();
}


bool Shadows::ReleaseGL()
{
	CoordinateFrame::Instance().Unload();

	for (unsigned int i = 0; i < MESH_NUM - 1; i++)
	{
		delete pSVMesh[i];
	}
	delete pTetraVBO;
	delete pRoomMesh;

	return GLResourceManager::Instance().Release();
}

bool Shadows::ReleaseApp()
{
	return true;
}



Shell *NewDemo()
{
	return new Shadows();
}

