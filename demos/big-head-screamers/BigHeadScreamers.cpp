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

#include "BigHeadScreamers.h"
#include "UnitTest.h"
#include "Misc.h"
#include <iostream>


const float TetraVertices[20] = {
	 0.0f    ,  1.0f    ,  0.0     , 0.5f, 1.0f,
	 0.0f    , -0.33333f,  0.94281f, 0.5f, 0.0f, 
	-0.81650f, -0.33333f, -0.47140f, 0.0f, 0.0f,
	 0.81650f, -0.33333f, -0.47140f, 1.0f, 0.0f,
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

const float Near = 10.0f;
const float Far = 40000.0f;



const char *szSkybox[] = {
	"data/textures/alpine_east.bmp",
	"data/textures/alpine_west.bmp",
	"data/textures/alpine_up.bmp",
	"data/textures/alpine_down.bmp",
	"data/textures/alpine_north.bmp",
	"data/textures/alpine_south.bmp",
};


BigHeadScreamers::BigHeadScreamers()
	: launcher(0.1f)
{
	bWireframe = false;

	pTetraVBO = NULL;
	pRoomMesh = NULL;

	iCurLight = 0;
	iNumLights = 1;
	light[0].fLightSpeed = 0.5f;
	light[0].fLightAngle = 0.0f;
	light[0].fLightHeight = 100.0f;
	light[0].fLightRadius = 1.0f;

	light[1].fLightSpeed = 0.6f;
	light[1].fLightAngle = 0.5 * M_PI;
	light[1].fLightHeight = -100.0f;
	light[1].fLightRadius = 1.2f;

	light[2].fLightSpeed = 0.35f;
	light[2].fLightAngle = M_PI;
	light[2].fLightHeight = 50.0f;
	light[2].fLightRadius = 1.5f;

	light[3].fLightSpeed = 0.4f;
	light[3].fLightAngle = M_PI + M_PI_2;
	light[3].fLightHeight = 40.0f;
	light[3].fLightRadius = 1.8f;


	bLightMove = true;

}

bool BigHeadScreamers::InitApp()
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

	UnitTestRun();

	return true;
}

bool BigHeadScreamers::LoadShaders()
{
	GLResourceManager &loader = GLResourceManager::Instance();
	for (unsigned int i = 0; i < E_NUM_PROGRAMS; i++)
	{
		if (!loader.LoadShaderFromFile(shaders[i*2+0], shaders[i*2+1],
			uiProgram[i]))
			return false;
	}
	return true;
}

bool BigHeadScreamers::InitGL()
{
	GLResourceManager &loader = GLResourceManager::Instance();

	// Load Shaders
	if (!LoadShaders())
		return false;

	// Load font used for overlay rendering
	if (!ttFont.LoadFont("data/fonts/LucidaBrightDemiBold.ttf", 20))
		return false;	

	if (!loader.LoadTextureFromFile("data/textures/crate-base.bmp",
		uiBGTexture, GL_LINEAR, GL_LINEAR))
		return false;
	
	unsigned int handle3DSFile;
	if (!loader.Load3DSFile("data/scenes/primitives.3ds", handle3DSFile))
		return false;


	// Load outer room
	pRoomMesh = new Mesh(loader.FindMesh(handle3DSFile, "Box"));
	

	CalculateBoundaries(handle3DSFile);


	pTetraVBO = new IndexedVBO((void *)TetraVertices, sizeof(float) * 5, 4,
	                           (void *)TetraIndices, 12);
	pTetraVBO->SetTexCoordData(sizeof(float) * 3);


	if (!fpsGraph.Init(4.0f, 3000, -0.98f, 0.60f, -0.6f, 0.95f, false))
	{
		return false;
	}
	
	if (!SkyBox::Instance().Init())
	{
		return false;
	}

	if (!alpinCubeMap.Init(szSkybox))
	{
		return false;
	}


	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );

	Resize(ShellGet(SHELL_WIDTH), ShellGet(SHELL_HEIGHT));

	glDisable(GL_DEPTH_TEST);

	// Hack
	fMaxDistance *= 3.0f;

	fpsCamera.Init(fDefDistance, -20.0f);

	timer.Start();
	return true;
}

void BigHeadScreamers::CalculateBoundaries(int index3ds)
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

bool BigHeadScreamers::Resize(unsigned int width, unsigned int height)
{
	//printf("Reshaping Screen to %dx%d\n", width, height);
	// Select and setup the projection matrix
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluPerspective( 65.0f, (GLfloat)width/(GLfloat)height, Near, Far);
	return true;
}



bool BigHeadScreamers::Render()
{
	/* Input - timer and fps graph */
	timer.Update();

	float dt = timer.GetDeltaTime();
	fpsGraph.Input(1.0f / dt, timer.GetTime());

	/* User input */
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
		light[iCurLight].fLightHeight += 50.0f * timer.GetDeltaTime();
	}
	if (KeyPressing(KEY_E))
	{
		light[iCurLight].fLightHeight -= 50.0f * timer.GetDeltaTime();
	}
	if (KeyPressing(KEY_W))
	{
		light[iCurLight].fLightRadius += timer.GetDeltaTime();
	}
	if (KeyPressing(KEY_S))
	{
		light[iCurLight].fLightRadius -= timer.GetDeltaTime();
	}
	if (KeyPressed(KEY_A))
	{
		if (iNumLights > 1)
			iNumLights--;
	}
	if (KeyPressed(KEY_D))
	{
		if (iNumLights < MAX_LIGHTS)
			iNumLights++;
	}
	launcher.Update(dt);
	if (LeftClick() || KeyPressing(KEY_SPACE))
	{
		launcher.Fire(fpsCamera);
	}

	
	// Update light positions
	if (bLightMove)
	{
		for (unsigned int i = 0; i < MAX_LIGHTS; i++)
		{
			light[i].fLightAngle += timer.GetDeltaTime();

			light[i].UpdatePos(fMinDistance);
		}
	}

	/* Rendering */
	// Clear color and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);

	// Select and setup the modelview matrix
	glMatrixMode( GL_MODELVIEW );

	glEnable(GL_TEXTURE_CUBE_MAP_EXT); 
	//glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, uiBGTexture);
	//glUseProgram(uiProgram[E_LOOKUP]);

	SkyBox::Instance().Render(alpinCubeMap, fpsCamera.GetAlpha(),
		fpsCamera.GetBeta(), Far / 10.0f);


	fpsCamera.Update(this, timer.GetDeltaTime());
	fpsCamera.LoadMatrix();

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, uiBGTexture);

	/*glEnable(GL_CULL_FACE);

	RenderRoom(uiProgram[E_DIFFUSE]);

	glDisable(GL_CULL_FACE);*/

	RenderScene();

	RenderGrenades();

	// Render Light
	/*for (unsigned int i = 0; i < iNumLights; i++)
	{
		DrawLightMarker(light[i].fPos);
	}*/

	//glDisable(GL_DEPTH_TEST);
	glClear(GL_DEPTH_BUFFER_BIT);

	DrawCoordinateFrame();

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
			ttFont.SDL_GL_RenderText(color, &position, "%.1f",  1.0f / timer.GetDeltaTime());
		}
		else if (iShowInfo == 2)
		{
			ttFont.SDL_GL_RenderText(color, &position, "FPS=%.1f",  1.0f / timer.GetDeltaTime());
			position.y -= position.h * 1.2f;

			ttFont.SDL_GL_RenderText(color, &position, "alpha=%.1f", fpsCamera.GetAlpha());
			position.y -= position.h * 1.2f;

			ttFont.SDL_GL_RenderText(color, &position, "beta=%.1f", fpsCamera.GetBeta());
			position.y -= position.h * 1.2f;

			const Vector3 tr = fpsCamera.GetTranslation();
			ttFont.SDL_GL_RenderText(color, &position, "t=[%.1f,%.1f,%.1f]", tr[0], tr[1], tr[2]);
			position.y -= position.h * 1.2f;

			ttFont.SDL_GL_RenderText(color, &position, "#=%d", uiInfPlaneVertices);
			position.y -= position.h * 1.2f;

			for (int i = 0; i < uiInfPlaneVertices; i++)
			{
				ttFont.SDL_GL_RenderText(color, &position, "w[%d]=[%.1f,%.1f,%.1f]", i, vGround[i][0], vGround[i][1], vGround[i][2]);
				position.y -= position.h * 1.2f;
			}
		}

		TTFont::glDisable2D();
		glDisable(GL_BLEND);
	}
	
	return true;
}

void BigHeadScreamers::RenderGrenades()
{
	float yellow[] = { 1.0f, 1.0f, 0.0f, 1.0f };
	GLuint shader = uiProgram[E_LOOKUP_COLOR];
	glUseProgram(shader);
	glUniform4fv(GetUniLoc(shader, "Color"), 1, yellow);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glPushMatrix();
	fpsCamera.LoadMatrix();

	//const list<Grenade> &list = launcher.GetGrenades();
	list<Grenade>::const_iterator iter;
	for (iter = launcher.GetGrenades().begin();
		iter != launcher.GetGrenades().end(); iter++)
	{
		glPushMatrix();
		const Point3 &pos = iter->GetPosition();
		glTranslatef(pos[0], pos[1], pos[2]);
		glScalef(10.0f, 10.0f, 10.0f);
		pTetraVBO->Render(GL_TRIANGLES);
		glPopMatrix();
	}
	glPopMatrix();

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void BigHeadScreamers::RenderRoom(GLuint program)
{
	/* Draw mesh */
	glUseProgram(program);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glCullFace(GL_BACK);

	if (program == uiProgram[E_DIFFUSE])
	{
		glUniform1i(GetUniLoc(program, "NumLights"), iNumLights);
		glUniform4fv(GetUniLoc(program, "Color"), 1, GrayColor);
	}
	PrintOpenGLError();

	pRoomMesh->GetVBO()->Render(GL_TRIANGLES);

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

#define DRAW_PLANE4(X, T, MODE) \
{\
	float temp[] = {\
		X[0][0], X[0][1], X[0][2], X[0][3], T, T, \
		X[1][0], X[1][1], X[1][2], X[1][3], 0.0f, T,\
		X[2][0], X[2][1], X[2][2], X[2][3], 0.0f, 0.0f,\
		X[3][0], X[3][1], X[3][2], X[3][3], T, 0.0f\
	};\
	glVertexPointer(4, GL_FLOAT, sizeof(float) * 6, temp);\
	glTexCoordPointer(2, GL_FLOAT, sizeof(float) * 6, temp + 4);\
	glDrawArrays(MODE, 0, 4);\
}

#define DRAW_INFINITE_PLANE(X, T, N, MODE) \
{\
	float temp[] = {\
		X[0][0], X[0][1], X[0][2], T, T, \
		X[1][0], X[1][1], X[1][2], 0.0f, T,\
		X[2][0], X[2][1], X[2][2], 0.0f, 0.0f,\
		X[3][0], X[3][1], X[3][2], T, 0.0f\
	};\
	glVertexPointer(3, GL_FLOAT, sizeof(float) * 5, temp);\
	glTexCoordPointer(2, GL_FLOAT, sizeof(float) * 5, temp + 3);\
	glDrawArrays(MODE, 0, N);\
}

void BigHeadScreamers::RenderScene()
{
	float proj[16], mview[16];

	Vector4 v[4];

	float t = 100.0f;
	float w = 0.0001f;
	v[0] = Vector4(1.0f, 0.0f, 1.0f, w);
	v[1] = Vector4(-1.0f, 0.0f, 1.0f, w);
	v[2] = Vector4(-1.0f, 0.0f, -1.0f, w);
	v[3] = Vector4(1.0f, 0.0f, -1.0f, w);

	//glMatrixMode( GL_PROJECTION );
	glGetFloatv(GL_PROJECTION_MATRIX, proj);
	//glMatrixMode( GL_MODELVIEW );
	glGetFloatv(GL_MODELVIEW_MATRIX, mview);

	/*DebugMatrix4 mp(proj);
	DebugMatrix4 mv(mview);

	Matrix4 ip = mp.Inverse(0.1);

	Matrix4 ii = ip * mp;*/

	Matrix4 invProjView = (Matrix4(proj) * Matrix4(mview)).Inverse(0.1);

	//Matrix4 ii = invProjView * Matrix4(proj) * Matrix4(mview);


	Vector3 eyePos = -fpsCamera.GetTranslation();
	const float plane[] = { 0.0f, 1.0f, 0.0f, 0.0f };
	uiInfPlaneVertices = InfinitePlane(vGround, plane, invProjView, eyePos, Far);


	Vector4 u[4];
	Vector4 x[4];
	for (unsigned int i = 0; i < 4; i++)
	{
		Projection(u[i], proj, mview, v[i]);
		ZeroZ(x[i], u[i]);
	}

	if (KeyPressed(KEY_1))
	{
		cout << "Modelview matrix\n";
		DebugMatrix4::GLPrint(mview, "%5.2f");
		cout << "Projection matrix\n";
		DebugMatrix4::GLPrint(proj, "%5.2f");

		for (unsigned int i = 0; i < 4; i++)
		{
			printf("Projection of [%.1f, %.1f, %.1f, %.1f] is "\
				"[%.1f, %.1f, %.1f, %.1f]\n",
				v[i][0], v[i][1], v[i][2], v[i][3],
				u[i][0], u[i][1], u[i][2], u[i][3]);
		}
	}

	glEnableClientState(GL_VERTEX_ARRAY);	
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	/*glUseProgram(uiProgram[E_INFINITY]);

	float repeat[] = { 0.01f, 0.01f };
	glUniform2fv(GetUniLoc(uiProgram[E_INFINITY], "TexRepeat"), 1, repeat);
	glUniform2fv(GetUniLoc(uiProgram[E_INFINITY], "TexOffset"), 1, &eyePos[0]);

	DRAW_INFINITE_PLANE(vGround, t, uiInfPlaneVertices, GL_TRIANGLE_FAN)*/

	glUseProgram(uiProgram[E_LOOKUP]);
	DRAW_PLANE4(v, t, GL_TRIANGLE_FAN)

	glUseProgram(uiProgram[E_IDENTITY]);
	float color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glUniform4fv(GetUniLoc(uiProgram[E_IDENTITY], "Color"), 1, color);

	//DRAW_PLANE4(u, t, GL_LINE_LOOP)
	// X has zeroed z component so it's unaffected by depth test
	DRAW_PLANE4(x, t, GL_LINE_LOOP)

	glDisableClientState(GL_VERTEX_ARRAY);	
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void BigHeadScreamers::Projection(Vector4 &u, const float *proj, const float *mview, const Vector4 &v)
{
	u = DebugMatrix4::GLMult(mview, v);
	u = DebugMatrix4::GLMult(proj, u);
}

void BigHeadScreamers::ZeroZ(Vector4 &w, const Vector4 &u)
{
	w = Vector4(u[0], u[1], 0.0f, u[3]);
}


void BigHeadScreamers::DrawLightMarker(float *lightPos)
{
	float white[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glUseProgram(uiProgram[E_UNIFORM]);
	glUniform4fv(GetUniLoc(uiProgram[E_UNIFORM], "Color"), 1, white);

	glTranslatef(lightPos[0], lightPos[1], lightPos[2]);
	glScalef(10.0f, 10.0f, 10.0f);
	glEnableClientState(GL_VERTEX_ARRAY);
	pTetraVBO->Render(GL_TRIANGLES);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void BigHeadScreamers::DrawCoordinateFrame()
{
	glUseProgram(uiProgram[E_COLOR_OFFSET]);
	float offset[] = { -0.9f, -0.9f };
	glUniform2fv(GetUniLoc(uiProgram[E_COLOR_OFFSET], "Offset"), 1, offset);

	//fpsCamera.LoadMatrix();
	glLoadIdentity();

	glTranslatef(0.0f, 0.0f, -fDefDistance);
	glRotatef(fpsCamera.GetBeta(), 1.0f, 0.0f, 0.0f);
	glRotatef(fpsCamera.GetAlpha(), 0.0f, 1.0f, 0.0f);

	CoordinateFrame::Instance().Render();
}


bool BigHeadScreamers::ReleaseGL()
{
	CoordinateFrame::Instance().Unload();

	delete pTetraVBO;
	delete pRoomMesh;

	return GLResourceManager::Instance().Release();
}

bool BigHeadScreamers::ReleaseApp()
{
	return true;
}

Shell *NewDemo()
{
	return new BigHeadScreamers();
}


