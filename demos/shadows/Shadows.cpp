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
#include "TTFont.h"
#include "Keys.h"

#include <math.h>
#include <stdio.h>

#include "GLResourceManager.h"
#include "VBO.h"
#include "CoordinateFrame.h"

#include "Mesh.h"
#include "ShadowVolume.h"
#include "version.h"

enum DisplayMode {
	E_SHADOW_VOLUMES,
	E_SHADOWS, 
	E_NO_SHADOWS, 
	E_DISPLAY_MODES 
};

enum Shaders {
	E_LOOKUP, 
	E_COLOR_OFFSET, 
	E_DIFFUSE, 
	E_UNIFORM, 
	E_SHADOW_VOLUME, 
	E_SHADOW, 
	E_NUM_PROGRAMS 
};

const char *shaders[] = {
	"data/shaders/Lookup.vert", "data/shaders/Lookup.frag",
	"data/shaders/ColorOffset.vert", "data/shaders/ColorOffset.frag",
	"data/shaders/DiffuseColor.vert", "data/shaders/DiffuseColor.frag",
	"data/shaders/UniformColor.vert", "data/shaders/UniformColor.frag",
	"data/shaders/ShadowVolume.vert", "data/shaders/ShadowVolume.frag",
	"data/shaders/Shadow.vert", "data/shaders/Shadow.frag",
};

enum Primitives {
	//MESH_BOX, 
	MESH_TORUS, 
	MESH_TORUS_KNOT, 
	MESH_TEAPOT, 
	//MESH_MONKEY, 
	MESH_NUM 
};

const char *meshes[] = {
	//"Box",
	"Torus",
	"TorusKnot",
	"Teapot",
	//"Suzanne",
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

//#define BENCHMARK_MODE

class Shadows : public SDLShell
{

protected:
	GLuint uiProgram[E_NUM_PROGRAMS];

	int iShowInfo;

	float y, x, t;
	bool wireframe;

	Timer timer;

	TTFont ttFont;

	//GLuint uiBGTexture;
	GLuint uiCoordFrameList;

	float maxDistance, minDistance;
	float defDistance;

	float boxSize;
	float distance;

	float fShadowExtent;
	float fMinShadowExtent;
	float fMaxShadowExtent;

	bool bInfiniteShadowVolume;
	float fLightSpeed;

	int iUpdateShadowPeriod;
	int iFrameCounter;

	int iCurrentObject;

	IndexedVBO *pTetraVBO;
	ShadowVolumeMesh *pSVMesh[MESH_NUM];
	Mesh *pRoomMesh;

	int eDisplayMode;

	bool LoadShaders();

	void DrawLightMarker(float xRot, float yRot, float *lightPos);
	void DrawCoordinateFrame(float xRot, float yRot);
	void CalculateBoundaries(int index3ds);

	void RenderGeometry(GLuint program);
	void RenderShadow();
	void RenderScene(float *light);
	void ShowShadowVolume();

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
	wireframe = false;
	unsigned int i;
	for (i = 0; i < MESH_NUM; i++)
	{
		pSVMesh[i] = NULL;
	}
	pTetraVBO = NULL;
	pRoomMesh = NULL;

	eDisplayMode = E_SHADOW_VOLUMES;

	bInfiniteShadowVolume = false;
	iUpdateShadowPeriod = 1;
	iFrameCounter = 0;

	iCurrentObject = 0;

	fLightSpeed = 0.75f;
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

	//if (!loader.LoadTextureFromFile("data/textures/perspec_warp_input_tex.bmp", uiBGTexture, GL_LINEAR, GL_LINEAR))
	//	return false;
	
	unsigned int prim3DSFile;
	if (!loader.Load3DSFile("data/scenes/primitives.3ds", prim3DSFile))
		return false;

	// Load outer room
	pRoomMesh = new Mesh(loader.FindMesh(prim3DSFile, "Box"));

	// Load Object Meshes and generate Shadow Volumes
	for (unsigned int i = 0; i < MESH_NUM; i++)
	{
		pSVMesh[i] = new ShadowVolumeMesh(loader.FindMesh(prim3DSFile, meshes[i]));
	}

	//pSVMesh[MESH_BOX] = new ShadowVolumeMesh(loader.FindMesh(prim3DSFile, "Box"), 0.1f, true);

	//unsigned int monkey3DSFile;
	//if (!loader.Load3DSFile("data/scenes/monkey.3ds", monkey3DSFile))
	//	return false;

	//pSVMesh[MESH_MONKEY] = new ShadowVolumeMesh(loader.FindMesh(monkey3DSFile, "Suzanne"), 60.0f);


	CalculateBoundaries(prim3DSFile);

	fMinShadowExtent = 0.0f;
	fMaxShadowExtent = 2.0f * maxDistance;
	fShadowExtent = fMaxShadowExtent;

	pTetraVBO = new IndexedVBO((void *)TetraVertices, sizeof(float) * 3, 4, (void *)TetraIndices, 12);


	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );

	Resize(ShellGet(SHELL_WIDTH), ShellGet(SHELL_HEIGHT));

	glDisable(GL_DEPTH_TEST);

	timer.Start();
	return true;
}

void Shadows::CalculateBoundaries(int index3ds)
{
	GLResourceManager &loader = GLResourceManager::Instance();

	// Calculating maximum and minimum range for zoom
	boxSize = loader.MeshSize(index3ds, "Box");

	maxDistance = 0.5f * boxSize;
	
	float d = minDistance = loader.MeshSize(index3ds, "Torus");
	d = loader.MeshSize(index3ds, "TorusKnot");
	if (d > minDistance)
		minDistance = d;
	d = loader.MeshSize(index3ds, "Teapot");
	if (d > minDistance)
		minDistance = d;

	distance = defDistance = 0.5f * (maxDistance + minDistance);

	CoordinateFrame::Instance().Make(maxDistance);
}

bool Shadows::Resize(unsigned int width, unsigned int height)
{
	//printf("Reshaping Screen to %dx%d\n", width, height);
	// Select and setup the projection matrix
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluPerspective( 65.0f, (GLfloat)width/(GLfloat)height, 1.0f, 2000.0f );
	return true;
}

bool Shadows::Render()
{
	timer.Update();

	if (KeyPressed(KEY_SPACE))
	{
		iCurrentObject = (iCurrentObject + 1) % (int)MESH_NUM;
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
#ifdef BENCHMARK_MODE
	if (KeyPressed(KEY_9))
	{
		bInfiniteShadowVolume = !bInfiniteShadowVolume;
	}
#endif
	if (KeyPressed(KEY_0))
	{
		wireframe = !wireframe;
	}
	if (KeyPressing(KEY_UP))
	{
		if ((distance -= timer.GetDeltaTime() * 150.0f) < minDistance)
			distance = minDistance;
	}
	if (KeyPressing(KEY_DOWN))
	{
		if ((distance += timer.GetDeltaTime() * 150.0f) > maxDistance)
			distance = maxDistance;
	}
#ifdef BENCHMARK_MODE
	if (KeyPressed(KEY_1))
	{
		if (iUpdateShadowPeriod > 1)
		{
			iUpdateShadowPeriod--;
		}
	}
	if (KeyPressed(KEY_2))
	{
		if (iUpdateShadowPeriod < 5)
			iUpdateShadowPeriod++;
	}
#endif

	y += 0.002f * (GetPointer()->GetMotionX() + GetPointer()->GetInertiaX());
	x -= 0.002f * (GetPointer()->GetMotionY() + GetPointer()->GetInertiaY());
	if (GetPointer()->TimeSinceLastInput() > 10.0f)
		t += timer.GetDeltaTime();

	float yRot = y + 20.0f * t;
	float xRot = x + 20.0f * sin(t);

	// Clear color buffer

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	/* Render background quad */
	/*glDisable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);

	glUseProgram(uiProgram[E_LOOKUP]);
	glBindTexture(GL_TEXTURE_2D, uiBGTexture);

	TTFont::glEnable2D();
	RenderQuad(0, 0, ShellGet(SHELL_WIDTH), ShellGet(SHELL_HEIGHT), 0,0,1,1);
	TTFont::glDisable2D();*/

	glEnable(GL_DEPTH_TEST);

	// Select and setup the modelview matrix
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	glTranslatef(0.0f, 0.0f, -distance);
	glRotatef(xRot, 1.0f, 0.0f, 0.0f);
	glRotatef(yRot, 0.0f, 1.0f, 0.0f);

	float light[4] = {
		minDistance * cos(timer.GetTime() * fLightSpeed),
		minDistance / 2,
		minDistance * sin(timer.GetTime() * fLightSpeed),
		1.0f
	};


	//glEnable(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, uiBGTexture);

	// Draw scene (including shadow if enabled)
	RenderScene(light);

	// Render Light
	DrawLightMarker(xRot, yRot, light);

	//glDisable(GL_DEPTH_TEST);
	glClear(GL_DEPTH_BUFFER_BIT);

	DrawCoordinateFrame(xRot, yRot);

	if (iShowInfo)
	{
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

		position.x = 0;
		position.y = ShellGet(SHELL_HEIGHT) - 40;

		if (iShowInfo == 1)
		{
			char text[40];
			sprintf(text, "%.1f",  1.0f / timer.GetDeltaTime());
			ttFont.SDL_GL_RenderText(text, color, &position);
		}
		else if (iShowInfo == 2)
		{
			char text[40];
			sprintf(text, "FPS=%.1f",  1.0f / timer.GetDeltaTime());
			ttFont.SDL_GL_RenderText(text, color, &position);
			position.y -= position.h * 1.2f;
			sprintf(text, "Extent=%.1f", fShadowExtent);
			ttFont.SDL_GL_RenderText(text, color, &position);
			position.y -= position.h * 1.2f;
			sprintf(text, "VBO=%d, %d", pSVMesh[iCurrentObject]->GetVBO()->GetCount(),
				pSVMesh[iCurrentObject]->GetShadowVolumeVBO()->GetCount());
			ttFont.SDL_GL_RenderText(text, color, &position);
			position.y -= position.h * 1.2f;
			sprintf(text, "Infinite Shadow Volume=%d", bInfiniteShadowVolume ? 1 : 0);
			ttFont.SDL_GL_RenderText(text, color, &position);
			position.y -= position.h * 1.2f;
		}
		

		TTFont::glDisable2D();
		glDisable(GL_BLEND);
	}
	iFrameCounter++;
	
	return true;
}


void Shadows::RenderGeometry(GLuint program)
{
	/* Draw mesh */
	glUseProgram(program);

	float bgColor[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	float objColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	glCullFace(GL_FRONT);
	glUniform4fv(GetUniLoc(program, "Color"), 1, bgColor);
	pRoomMesh->GetVBO()->Render(GL_TRIANGLES);

	glCullFace(GL_BACK);
	glUniform4fv(GetUniLoc(program, "Color"), 1, objColor);
	pSVMesh[iCurrentObject]->GetVBO()->Render(wireframe ? GL_LINES : GL_TRIANGLES);

	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void Shadows::ShowShadowVolume()
{
	glUseProgram(uiProgram[E_SHADOW_VOLUME]);

	glUniform1f(GetUniLoc(uiProgram[E_SHADOW_VOLUME], "ShadowExtent"), fShadowExtent);
	glUniform1i(GetUniLoc(uiProgram[E_SHADOW_VOLUME], "InfiniteShadowVolume"), bInfiniteShadowVolume);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	pSVMesh[iCurrentObject]->GetShadowVolumeVBO()->Render(wireframe ? GL_LINES : GL_TRIANGLES);

	glDisableClientState(GL_NORMAL_ARRAY);	
	glDisableClientState(GL_VERTEX_ARRAY);
}

void Shadows::RenderShadow()
{
	// Update shadow every "iUpdateShadowPeriod" frames;
	if (iFrameCounter % iUpdateShadowPeriod)
		return;

	// store current OpenGL state
	glPushAttrib(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_POLYGON_BIT | GL_STENCIL_BUFFER_BIT);

	glUseProgram(uiProgram[E_SHADOW_VOLUME]);

	glUniform1f(GetUniLoc(uiProgram[E_SHADOW_VOLUME], "ShadowExtent"), fShadowExtent);
	glUniform1i(GetUniLoc(uiProgram[E_SHADOW_VOLUME], "InfiniteShadowVolume"), bInfiniteShadowVolume);

	glClear(GL_STENCIL_BUFFER_BIT);

	glColorMask(0, 0, 0, 0); // do not write to the color buffer
	glDepthMask(0); // do not write to the depth (Z) buffer
	glEnable(GL_CULL_FACE); // cull faces (back or front)
	glEnable(GL_STENCIL_TEST); // enable stencil testing

	// set the reference stencil value to 0
	glStencilFunc(GL_ALWAYS, 0, ~0);

	// increment the stencil value on Z fail
	glStencilOp(GL_KEEP, GL_INCR, GL_KEEP);

	// draw only the back faces of the shadow volume
	glCullFace(GL_FRONT);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	pSVMesh[iCurrentObject]->GetShadowVolumeVBO()->Render(GL_TRIANGLES);

	// decrement the stencil value on Z fail
	glStencilOp(GL_KEEP, GL_DECR, GL_KEEP);

	// draw only the front faces of the shadow volume
	glCullFace(GL_BACK);
	pSVMesh[iCurrentObject]->GetShadowVolumeVBO()->Render(GL_TRIANGLES);

	glDisableClientState(GL_NORMAL_ARRAY);	
	glDisableClientState(GL_VERTEX_ARRAY);

	// restore OpenGL state
	glPopAttrib();
}

void Shadows::RenderScene(float *light)
{
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_POSITION, light);

	switch (eDisplayMode)
	{
	default:
	case E_NO_SHADOWS:
		RenderGeometry(uiProgram[E_DIFFUSE]);
		break;
	case E_SHADOW_VOLUMES:
		RenderGeometry(uiProgram[E_DIFFUSE]);
		ShowShadowVolume();
		break;
	case E_SHADOWS:
		//glPushAttrib(GL_DEPTH_BUFFER_BIT | GL_LIGHTING_BIT | GL_STENCIL_BUFFER_BIT);

		RenderGeometry(uiProgram[E_DIFFUSE]);
		//RenderGeometry(uiProgram[E_UNIFORM]);

		RenderShadow();

		// re-draw the model with the light enabled only where
		// it has previously been drawn
		//glClear(GL_DEPTH_BUFFER_BIT);
		glDepthFunc(GL_EQUAL);

		// update the color only where the stencil value is 0
		glEnable(GL_STENCIL_TEST);
		//glStencilFunc(GL_EQUAL, 0, ~0);
		glStencilFunc(GL_EQUAL, 1, ~0);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

		glEnable(GL_BLEND);
		//glBlendFunc(GL_ONE, GL_ONE);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//RenderGeometry(uiProgram[E_DIFFUSE]);
		RenderGeometry(uiProgram[E_SHADOW]);

		glDisable(GL_STENCIL_TEST);
		glDisable(GL_BLEND);
		glDepthFunc(GL_LESS);

		// restore OpenGL state
		//glPopAttrib();
		break;
	}
}


void Shadows::DrawLightMarker(float xRot, float yRot, float *lightPos)
{
	float white[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glUseProgram(uiProgram[E_UNIFORM]);
	glUniform4fv(GetUniLoc(uiProgram[E_UNIFORM], "Color"), 1, white);

	glLoadIdentity();

	glTranslatef(0.0f, 0.0f, -distance);
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

	glTranslatef(0.0f, 0.0f, -defDistance);
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

