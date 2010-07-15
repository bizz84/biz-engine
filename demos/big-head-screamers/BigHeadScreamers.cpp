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

#include "BigHeadScreamers.h"
#include "UnitTest.h"
#include "Misc.h"
#include <iostream>


static const float TetraVertices[20] = {
	 0.0f    ,  1.0f    ,  0.0     , 0.5f, 1.0f,
	 0.0f    , -0.33333f,  0.94281f, 0.5f, 0.0f, 
	-0.81650f, -0.33333f, -0.47140f, 0.0f, 0.0f,
	 0.81650f, -0.33333f, -0.47140f, 1.0f, 0.0f,
};

static const int TetraIndices[12] = {
	0, 1, 2,
	0, 2, 3,
	0, 3, 1,
	1, 2, 3
};


static const float Near = 1.0f;
static const float Far = 100000.0f;
static const float Height = 20.0f;

static const char *szSkybox[] = {
	"data/textures/alpine_east.bmp",
	"data/textures/alpine_west.bmp",
	"data/textures/alpine_up.bmp",
	"data/textures/alpine_down.bmp",
	"data/textures/alpine_north.bmp",
	"data/textures/alpine_south.bmp",
};


static const char *shaders[] = {
	"data/shaders/Lookup.vert", "data/shaders/Lookup.frag",
	"data/shaders/LookupColor.vert", "data/shaders/LookupColor.frag",
	"data/shaders/ColorOffset.vert", "data/shaders/ColorOffset.frag",
	"data/shaders/UniformColor.vert", "data/shaders/UniformColor.frag",
};


BigHeadScreamers::BigHeadScreamers()
	: launcher(0.1f),
	iShowInfo(0),
	uiBGTexture(~0),
	pTetraVBO(NULL)
{

}

bool BigHeadScreamers::InitApp()
{
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

	//UnitTestRun();

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

	if (!ground.Init())
		return false;

	// Load Shaders
	if (!LoadShaders())
		return false;

	// Load font used for overlay rendering
	if (!ttFont.LoadFont("data/fonts/LucidaBrightDemiBold.ttf", 20))
		return false;	

	// Load texture for ground
	if (!loader.LoadTextureFromFile("data/textures/crate-base.bmp",
		uiBGTexture, GL_LINEAR, GL_LINEAR))
		return false;
	
	// Initialize FPS graph
	if (!fpsGraph.Init(4.0f, 3000, -0.98f, 0.60f, -0.6f, 0.95f, false))
		return false;
	
	// Initialize skybox singleton
	if (!SkyBox::Instance().Init())
		return false;

	// Load cubemap for skybox
	if (!alpinCubeMap.Init(szSkybox))
		return false;

	CoordinateFrame::Instance().Make(400);

	pTetraVBO = new IndexedVBO((void *)TetraVertices, sizeof(float) * 5, 4,
	                           (void *)TetraIndices, 12);
	pTetraVBO->SetTexCoordData(sizeof(float) * 3);




	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );

	Resize(ShellGet(SHELL_WIDTH), ShellGet(SHELL_HEIGHT));

	glDisable(GL_DEPTH_TEST);


	fpsCamera.Init(5.0f * Height, -Height);
	//fpsCamera.Init(0.1 * Far, -Height);

	timer.Start();
	return true;
}

bool BigHeadScreamers::Resize(unsigned int width, unsigned int height)
{
	//printf("Reshaping Screen to %dx%d\n", width, height);
	// Select and setup the projection matrix
	glMatrixMode( GL_PROJECTION );
	float fov = 65.0f * M_PI / 180.0f;
	float aspect = (GLfloat)width/(GLfloat)height;
	//Matrix4 proj = ProjectionRH(fov, aspect, Near, Far);
	Matrix4 proj = ProjectionRHInfinite(fov, aspect, Near);
	glLoadTransposeMatrixf(proj.data());
	// Calculate inverse (used to draw infinite plane)
	mInvProj = InverseProjectionRHInfinite(fov, 1.0/aspect, Near);

	// Alternatively use standard gluPerspective method
	//glLoadIdentity();
	//gluPerspective( 65.0f, (GLfloat)width/(GLfloat)height, Near, Far);

	return true;
}



bool BigHeadScreamers::Render()
{
	/* Input - timer and fps graph */
	timer.Update();

	float dt = timer.GetDeltaTime();
	fpsGraph.Input(1.0f / dt, timer.GetTime());

	/* User input */
	launcher.Update(dt);
	if (LeftClick() || KeyPressing(KEY_SPACE))
	{
		launcher.Fire(fpsCamera);
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
	fpsCamera.LoadMatrixNoXZ();


	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, uiBGTexture);


	// Use this to query matrices and check the inverse is calculated
	//correctly
	/*float p[16], m[16];
	glGetFloatv(GL_PROJECTION_MATRIX, p);
	glGetFloatv(GL_MODELVIEW_MATRIX, m);
	Matrix4 P = Matrix4(p).Transpose();
	Matrix4 M = Matrix4(m).Transpose();*/

	Matrix4 rot = AlphaBetaRotation(fpsCamera.GetAlpha(), fpsCamera.GetBeta());
	Vector3 translationNoXZ = Vector3(0.0f, fpsCamera.GetTranslation()[1], 0.0f);
	Matrix4 projViewInv = InverseMVP(mInvProj, translationNoXZ, rot);

	ground.Render(projViewInv, -fpsCamera.GetTranslation(), Far);

	fpsCamera.LoadMatrix();
	RenderGrenades();


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

			const Vector3 tr = -fpsCamera.GetTranslation();
			ttFont.SDL_GL_RenderText(color, &position, "pos=[%.1f,%.1f,%.1f]", tr[0], tr[1], tr[2]);
			position.y -= position.h * 1.2f;

			ttFont.SDL_GL_RenderText(color, &position, "#=%d", ground.VisibleVertices());
			position.y -= position.h * 1.2f;

			for (int i = 0; i < ground.VisibleVertices(); i++)
			{
				ttFont.SDL_GL_RenderText(color, &position, "w[%d]=[%.1f,%.1f,%.1f]", i, ground[i][0], ground[i][1], ground[i][2]);
				position.y -= position.h * 1.2f;
			}
		}

		TTFont::glDisable2D();
		glDisable(GL_BLEND);
	}
	
	return true;
}

void BigHeadScreamers::RenderGrenades() const
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


void BigHeadScreamers::DrawCoordinateFrame() const
{
	float offset[] = { -0.9f, -0.9f };
	glUseProgram(uiProgram[E_COLOR_OFFSET]);
	glUniform2fv(GetUniLoc(uiProgram[E_COLOR_OFFSET], "Offset"), 1, offset);

	glLoadIdentity();

	glTranslatef(0.0f, 0.0f, -250);
	//glTranslatef(0.0f, 0.0f, -fDefDistance);
	glRotatef(fpsCamera.GetBeta(), 1.0f, 0.0f, 0.0f);
	glRotatef(fpsCamera.GetAlpha(), 0.0f, 1.0f, 0.0f);

	CoordinateFrame::Instance().Render();
}


bool BigHeadScreamers::ReleaseGL()
{
	CoordinateFrame::Instance().Unload();

	delete pTetraVBO;
	//delete pRoomMesh;

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


