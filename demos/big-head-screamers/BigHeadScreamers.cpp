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

// Near, Far values for projection matrix and distance from plane
static const float Near = 1.0f;
static const float Far = 100000.0f;
static const float Height = 20.0f;

// Object representing a tetrahedron
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

// Credit for cubemaps:
// http://www.alusion-fr.com/
static const char *CubemapAlpine[] = {
	"data/textures/Skybox_alpine/alpine_east.bmp",
	"data/textures/Skybox_alpine/alpine_west.bmp",
	"data/textures/Skybox_alpine/alpine_up.bmp",
	"data/textures/Skybox_alpine/alpine_down.bmp",
	"data/textures/Skybox_alpine/alpine_north.bmp",
	"data/textures/Skybox_alpine/alpine_south.bmp",
};

static const char *CubemapCanyon[] = {
	"data/textures/Skybox_canyon/lnhcanyons1_fr.bmp",
	"data/textures/Skybox_canyon/lnhcanyons1_bk.bmp",
	"data/textures/Skybox_canyon/lnhcanyons1_up.bmp",
	"data/textures/Skybox_canyon/lnhcanyons1_dn.bmp",
	"data/textures/Skybox_canyon/lnhcanyons1_rt.bmp",
	"data/textures/Skybox_canyon/lnhcanyons1_lt.bmp",
};

static const char *CubemapFaesky[] = {
	"data/textures/Skybox_faesky/faesky02left.bmp",
	"data/textures/Skybox_faesky/faesky02right.bmp",
	"data/textures/Skybox_faesky/faesky02up.bmp",
	"data/textures/Skybox_faesky/faesky02down.bmp",
	"data/textures/Skybox_faesky/faesky02back.bmp",
	"data/textures/Skybox_faesky/faesky02front.bmp",
};

static const char *CubemapTaosky[] = {
	"data/textures/Skybox_taosky/skybox3_right.bmp",
	"data/textures/Skybox_taosky/skybox3_back.bmp",
	"data/textures/Skybox_taosky/skybox3_top.bmp",
	"data/textures/Skybox_taosky/skybox3_bottom.bmp",
	"data/textures/Skybox_taosky/skybox3_front.bmp",
	"data/textures/Skybox_taosky/skybox3_left.bmp",
};

static const char *CubemapMntsea[] = {
	"data/textures/Skybox_mntsea/lnhmtns1_fr.bmp",
	"data/textures/Skybox_mntsea/lnhmtns1_bk.bmp",
	"data/textures/Skybox_mntsea/lnhmtns1_up.bmp",
	"data/textures/Skybox_mntsea/lnhmtns1_dn.bmp",
	"data/textures/Skybox_mntsea/lnhmtns1_rt.bmp",
	"data/textures/Skybox_mntsea/lnhmtns1_lt.bmp",
};


static const char **Cubemaps[6] = {
	CubemapAlpine,
	CubemapCanyon,
	CubemapFaesky,
	CubemapTaosky,
	CubemapMntsea,
};

// Shaders
static const char *Shaders[] = {
	"data/shaders/Lookup.vert", "data/shaders/Lookup.frag",
	"data/shaders/LookupColor.vert", "data/shaders/LookupColor.frag",
	"data/shaders/ColorOffset.vert", "data/shaders/ColorOffset.frag"
};

// Credit for textures (licensed under Creative Commons):
// http://www.marcchehab.org/frozenmist
static const char *Textures[] = {
	"data/textures/Rore_floor-tiles2.bmp",
	"data/textures/TiZeta_pav2.bmp"
};



BigHeadScreamers::BigHeadScreamers()
	: ground(this),
	launcher(0.1f),
	iShowInfo(0),
	uiCurTexture(0),
	uiCurCubemap(0),
	fSetTime(0.0f),
	fRandomTime(0.0f),
	pTetraVBO(NULL),
	pReflectionFBO(NULL)
{
	// initialize random number generator
	Timer::InitRand();
}

BigHeadScreamers::~BigHeadScreamers()
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

	return true;
}

bool BigHeadScreamers::LoadShaders()
{
	GLResourceManager &loader = GLResourceManager::Instance();
	for (unsigned int i = 0; i < NUM_PROGRAMS; i++)
	{
		if (!loader.LoadShaderFromFile(Shaders[i<<1], Shaders[(i<<1)+1],
			uiProgram[i]))
			return false;
	}
	return true;
}

bool BigHeadScreamers::LoadTextures()
{
	GLResourceManager &loader = GLResourceManager::Instance();

	for (unsigned int i = 0; i < NUM_TEXTURES; i++)
	{
		// Load texture for ground
		if (!loader.LoadTextureFromFile(Textures[i],
			uiTexture[i], GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR))
			return false;
	}
}

bool BigHeadScreamers::LoadCubemaps()
{
	GLResourceManager &loader = GLResourceManager::Instance();

	for (unsigned int i = 0; i < NUM_CUBEMAPS; i++)
	{
		if (!cubemap[i].Init(Cubemaps[i]))
			return false;
	}
}

bool BigHeadScreamers::InitGL()
{
	GLResourceManager &loader = GLResourceManager::Instance();

	if (!ground.Init())
		return false;

	// Load Shaders
	if (!LoadShaders())
		return false;

	if (!LoadTextures())
		return false;

	if (!LoadCubemaps())
		return false;

	// Load font used for overlay rendering
	if (!ttFont.LoadFont("data/fonts/LucidaBrightDemiBold.ttf", 20))
		return false;	


	
	// Initialize FPS graph
	if (!fpsGraph.Init(4.0f, 3000, -0.98f, 0.60f, -0.6f, 0.95f, false))
		return false;
	
	// Initialize skybox singleton
	if (!SkyBox::Instance().Init())
		return false;

	// Initialize coordinate frame
	CoordinateFrame::Instance().Make(400);

	// Initialize vbo used for tetrahedron
	pTetraVBO = new IndexedVBO((void *)TetraVertices, sizeof(float) * 5, 4,
	                           (void *)TetraIndices, 12);
	pTetraVBO->SetTexCoordData(sizeof(float) * 3);

	// Initialize fbo used for drawing reflection
	pReflectionFBO = new FBO(GL_RGB, GL_UNSIGNED_SHORT_5_6_5, ShellGet(SHELL_WIDTH),
		ShellGet(SHELL_HEIGHT), true);


	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );

	Resize(ShellGet(SHELL_WIDTH), ShellGet(SHELL_HEIGHT));

	// As first element to be drawn is always the skybox
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
	// Select and setup the modelview matrix
	glMatrixMode( GL_MODELVIEW );
	return true;
}


void BigHeadScreamers::Input()
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
	fpsCamera.Update(this, timer.GetDeltaTime());

	if (KeyPressed(KEY_1))
		uiCurTexture = Prev(uiCurTexture, NUM_TEXTURES);
	if (KeyPressed(KEY_2))
		uiCurTexture = Next(uiCurTexture, NUM_TEXTURES);

	if (KeyPressed(KEY_3))
		uiCurCubemap = Prev(uiCurCubemap, NUM_CUBEMAPS);
	if (KeyPressed(KEY_4))
		uiCurCubemap = Next(uiCurCubemap, NUM_CUBEMAPS);


	// Ground input
	Matrix4 rot = AlphaBetaRotation(fpsCamera.GetAlpha(), fpsCamera.GetBeta());
	Vector3 translationNoXZ = Vector3(0.0f, fpsCamera.GetTranslation()[1], 0.0f);
	Matrix4 projViewInv = InverseMVP(mInvProj, translationNoXZ, rot);
	ground.Input(projViewInv, -fpsCamera.GetTranslation(), Far);

	// Some randomisation
	if (timer.GetTime() - fSetTime > fRandomTime)
	{
		fSetTime = timer.GetTime();
		fRandomTime = RandRange(5.0f, 15.0f);
		if ((rand() % 4) == 0)
			NextTexture();
		else
			NextCubemap();
	}
	
}


bool BigHeadScreamers::Render()
{
	Input();	
	
	/* Rendering */
	// Clear color and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Render skybox to framebuffer
	RenderSkyBox();

	glEnable(GL_DEPTH_TEST);

	// TODO: add code for rendering reflection to fbo
	RenderReflection();
	
	// sets modelview matrix to LoadMatrixNoXZ()
	RenderGround();

	// sets modelview matrix to LoadMatrix()
	RenderGrenades();

	// Clears depth buffer
	DrawCoordinateFrame();

	glDisable(GL_DEPTH_TEST);

	// Draws fps counter, fbos and overlays
	ShowInfo();
	
	return true;
}

void BigHeadScreamers::ShowInfo()
{
	if (iShowInfo)
	{
		RenderReflectionFBO();

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
		// TODO
		//glBlendFunc(GL_ONE, GL_ONE);
	}
}

void BigHeadScreamers::SkyBoxRotate() const 
{
	glLoadIdentity();
	// first rotate beta angle to avoid spin
	glRotatef(fpsCamera.GetBeta(), 1.0, 0.0, 0.0);
	// then rotate alpha angle to orient
	glRotatef(fpsCamera.GetAlpha(), 0.0, 1.0, 0.0);

	float scale = Far / 10.0f;
	glScalef(scale, scale, scale);
}


void BigHeadScreamers::RenderSkyBox() const
{
	glPushMatrix();
	
	SkyBoxRotate();

	SkyBox::Instance().Render(CurrentCubemap());
		
	glPopMatrix();
}

void BigHeadScreamers::RenderReflection() const
{
	pReflectionFBO->BindBuffer();
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	Vector3 eye = -fpsCamera.GetTranslation();
	eye[1] = -eye[1];

	Matrix4 mirror;
	// Note: treat this matrix as column major since it is then added
	// to the gl stack
	//mirror[0][1] = 0.0f;
	mirror[1][1] = -1.0f;
	//mirror[2][1] = 0.0f;
	//mirror[3][1] = 0.0f; // -2.0f * 0.0f
	
	SkyBoxRotate();
	glMultMatrixf(mirror.data());



	SkyBox::Instance().Render(CurrentCubemap());

	glEnable(GL_DEPTH_TEST);

	pReflectionFBO->UnbindBuffer();
}

void BigHeadScreamers::RenderGround()
{
	fpsCamera.LoadMatrixNoXZ();

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, CurrentTexture());

	// Set reflection texture (the shader in Ground expects it)
	glActiveTexture(GL_TEXTURE1);
	pReflectionFBO->BindTexture();
	glActiveTexture(GL_TEXTURE0);

	ground.Render(-fpsCamera.GetTranslation(), Far);
}




void BigHeadScreamers::RenderGrenades() const
{
	fpsCamera.LoadMatrix();
	
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

void BigHeadScreamers::RenderReflectionFBO()
{

	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	glLoadIdentity();

	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glLoadIdentity();

	glUseProgram(uiProgram[E_LOOKUP]);

	pReflectionFBO->BindTexture();

	RenderQuad2D(0.68f, -0.98f, 0.3f, 0.3f, 0.0f, 0.0f, 1.0f, 1.0f);

	glPopMatrix();

	glMatrixMode( GL_MODELVIEW );
	glPopMatrix();
}



void BigHeadScreamers::DrawCoordinateFrame() const
{
	glClear(GL_DEPTH_BUFFER_BIT);
	
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
	
	delete pReflectionFBO;

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


