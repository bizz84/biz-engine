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
#include "Misc.h"
#include "EnemyRendererAttrib.h"
#include "EnemyRendererBasic.h"
#include <iostream>

#include "Ground.h"
#include "AIManager.h"
#include "WeaponManager.h"
#include "ParticleRenderer.h"
#include "EnemyRendererAttrib.h"
#include "CollisionDetector.h"

#include "GrenadeRenderer.h"
#include "Settings.h"
#include "SkyBoxManager.h"

#include "UnitTest.h"


// Shaders
static const char *Shaders[] = {
	"data/shaders/Lookup.vert", "data/shaders/Lookup.frag",
	"data/shaders/ColorOffset.vert", "data/shaders/ColorOffset.frag"
};

// Credit for textures (licensed under Creative Commons):
// http://www.marcchehab.org/frozenmist
static const char *Textures[] = {
	"data/textures/TiZeta_pav2.bmp",
	"data/textures/Rore_floor-tiles2.bmp"
};


BigHeadScreamers::BigHeadScreamers() : 
	iShowInfo(0),
	uiCurTexture(0),
	fSetTime(0.0f),
	fRandomTime(0.0f),
	bReflectionFlag(false),
	pReflectionFBO(NULL),
	pAI(NULL),
	pWM(NULL),
	pExpR(NULL),
	pER(NULL),
	fCollisionTime(0.0f), // debug
	fEnemiesTime(0.0f)
{
	// initialize random number generator
	Timer::InitRand();

	enabled[S_COLLISIONS] = 1 + DETECTOR_SEGMENT_SPHERE;
	enabled[S_REFLECTION] = 1;
	enabled[S_INPUT] = 1;


	//TestList();
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
	return true;
}

void BigHeadScreamers::ReloadFBO()
{
	// Initialize fbo used for drawing reflection
	pReflectionFBO = auto_ptr<FBO>(new FBO(GL_RGB, GL_UNSIGNED_SHORT_5_6_5,
		ShellGet(SHELL_WIDTH), ShellGet(SHELL_HEIGHT), true));
}


bool BigHeadScreamers::InitGL()
{
	fpsCamera.Init(5.0f * 20.0f, -20.0f);

	GLResourceManager &loader = GLResourceManager::Instance();

	pGround = auto_ptr<Ground>(new Ground());
	if (!pGround->Init())
		return false;

	// Load Shaders
	if (!LoadShaders(Shaders, NUM_PROGRAMS))
		return false;

	if (!LoadTextures())
		return false;

	// Load font used for overlay rendering
	if (!ttFont.LoadFont("data/fonts/LucidaBrightDemiBold.ttf", 20))
		return false;	
	
	// Initialize FPS graph
	if (!fpsGraph.Init(4.0f, 3000, -0.98f, 0.60f, -0.6f, 0.95f, false))
		return false;

	// Initialize skybox singleton
	pSkyBoxManager = auto_ptr<SkyBoxManager>(new SkyBoxManager());

	// Initialize coordinate frame
	CoordinateFrame::Instance().Make(400);

	// Initialize fbo used for drawing reflection
	ReloadFBO();

	// Initialize AI 
	pAI = auto_ptr<AIManager>(new AIManager(fpsCamera.GetPosition()));

	// Initialize weapon system
	pWM = auto_ptr<WeaponManager>(new WeaponManager());

	// Initialize weapon renderer
	pExpR = auto_ptr<ParticleRenderer>(new ParticleRenderer());

	
	pER = auto_ptr<EnemyRendererAttrib>(new EnemyRendererAttrib());

	pDetector[DETECTOR_SEGMENT_SPHERE] = 
		auto_ptr<CollisionDetector>(new CPUSegmentSphereCollisionDetector(pWM.get(), pAI.get()));
	pDetector[DETECTOR_SPHERE_SPHERE] = 
		auto_ptr<CollisionDetector>(new CPUSphereSphereCollisionDetector(pWM.get(), pAI.get()));



	// GL state setup
	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );

	glPointSize(Settings::Instance().PointSize);

	// As first element to be drawn is always the skybox

	fFOV = Settings::Instance().Fov;

	Resize(ShellGet(SHELL_WIDTH), ShellGet(SHELL_HEIGHT));

	// These operations are performed in Input() but are set here to update the state
	// in case Input() is not used (benchmarking)
	fpsCamera.Update(this, 0.0f);
	pER->Update(pAI->GetData(), -fpsCamera.GetAlpha(), Settings::Instance().EnemyHeight);
	GroundInput();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glEnableClientState(GL_VERTEX_ARRAY);	
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	timer.Start();
	return true;
}

bool BigHeadScreamers::Resize(unsigned int width, unsigned int height)
{


	//printf("Reshaping Screen to %dx%d\n", width, height);
	// Select and setup the projection matrix
	glMatrixMode( GL_PROJECTION );
	float fov = fFOV * M_PI / 180.0f;
	float aspect = (GLfloat)width/(GLfloat)height;
	//Matrix4 proj = ProjectionRH(fov, aspect, Near, Far);
	Matrix4 proj = ProjectionRHInfinite(fov, aspect, Settings::Instance().Near);
	glLoadTransposeMatrixf(proj.data());
	// Calculate inverse (used to draw infinite plane)
	mInvProj = InverseProjectionRHInfinite(fov, 1.0/aspect, Settings::Instance().Near);

	// Alternatively use standard gluPerspective method
	//glLoadIdentity();
	//gluPerspective( fov, (GLfloat)width/(GLfloat)height, Near, Far);
	// Select and setup the modelview matrix
	glMatrixMode( GL_MODELVIEW );
	return true;
}

void BigHeadScreamers::GroundInput()
{
	// Ground input
	Matrix4 rot = AlphaBetaRotation(fpsCamera.GetAlpha() * M_PI / 180.0f, fpsCamera.GetBeta() * M_PI / 180.0f);
	Vector3 translationNoXZ = Vector3(0.0f, fpsCamera.GetTranslation()[1], 0.0f);
	Matrix4 projViewInv = InverseMVP(mInvProj, translationNoXZ, rot);
	pGround->Input(projViewInv, fpsCamera.GetPosition(), Settings::Instance().Far);
}

void BigHeadScreamers::Input()
{
	Timer inputTime; // timer for the execution time of this function
	/* Input - timer */
	timer.Update();

	float t = timer.GetTime();
	float dt = timer.GetDeltaTime();

	/* User Input */
	// Visual options input: ground and cubemap textures
	if (KeyPressed(KEY_1))
		uiCurTexture = Prev(uiCurTexture, NUM_TEXTURES);
	if (KeyPressed(KEY_2))
		uiCurTexture = Next(uiCurTexture, NUM_TEXTURES);

	if (KeyPressed(KEY_3))
		pSkyBoxManager->CubemapUpdate(false, t);
	if (KeyPressed(KEY_4))
		pSkyBoxManager->CubemapUpdate(true, t);

	if (KeyPressing(KEY_5))
	{
		if ((fFOV /= (1.0f + 0.25 * dt)) < 40.0f)
			fFOV = 40.0f;
		Resize(ShellGet(SHELL_WIDTH), ShellGet(SHELL_HEIGHT));
	}
	if (KeyPressing(KEY_6))
	{
		if ((fFOV *= (1.0f + 0.25 * dt)) > 150.0f)
			fFOV = 150.0f;
		Resize(ShellGet(SHELL_WIDTH), ShellGet(SHELL_HEIGHT));
	}
	if (KeyPressing(KEY_7))
	{
		fFOV = Settings::Instance().Fov;
		Resize(ShellGet(SHELL_WIDTH), ShellGet(SHELL_HEIGHT));
	}
	if (KeyPressed(KEY_8))
	{
		enabled[S_COLLISIONS] = Next(enabled[S_COLLISIONS], NUM_DETECTORS + 1);
	}
	if (KeyPressed(KEY_9))
	{
		enabled[S_REFLECTION] = Next(enabled[S_REFLECTION], 2);
	}
	if (KeyPressed(KEY_0))
	{
		enabled[S_INPUT] = Next(enabled[S_INPUT], 2);
	}

	// Change weapon
	if (ScrollDown())
	{
		pWM->NextWeapon();
		cout << "Switched to weapon " << pWM->CurrWeapon() << endl;
	}
	if (ScrollUp())
	{
		pWM->PrevWeapon();
		cout << "Switched to weapon " << pWM->CurrWeapon() << endl;
	}

	// Update camera position
	fpsCamera.Update(this, dt);

	GroundInput();

	if (enabled[S_INPUT])
	{
		// Loop through cubemaps
		if (Settings::Instance().MinRandomCycle != 0.0f && t - fSetTime > fRandomTime)
		{
			fSetTime = timer.GetTime();
			fRandomTime = RandRange(Settings::Instance().MinRandomCycle, Settings::Instance().MaxRandomCycle);
			pSkyBoxManager->CubemapUpdate(true, t);
		}
		pSkyBoxManager->Update(t)	;


		// Game input
		// Weapons input
		pWM->Input(dt, fpsCamera, LeftClick() || KeyPressing(KEY_SPACE));

		// AI input
		pAI->Input(t, dt, fpsCamera.GetPosition(), RightClick());

		if (enabled[S_COLLISIONS])
		{
			Timer temp;
			pDetector[enabled[S_COLLISIONS] - 1]->Run();
			fCollisionTime = temp.Update();
		}

		fEnemiesTime = 0.0f;

		pWM->UpdateState();
		pAI->UpdateState(fpsCamera.GetPosition());

		Timer temp;
		pER->Update(pAI->GetData(), -fpsCamera.GetAlpha(), Settings::Instance().EnemyHeight);
		fEnemiesTime = temp.Update();

		// Overlay update
		if (iShowInfo >= 2)
		{
			// Graph input
			fpsGraph.Input(1.0f / dt, t);
		}
	}

	fInputTime = inputTime.Update();
}



bool BigHeadScreamers::Render()
{
	Input();	
	
	/* Rendering */
	// First render mirrored content to FBO
	if (enabled[S_REFLECTION])
		RenderReflection();

	// Then, render normally
	RenderScene();

	// Draws fps counter, fbos and overlays
	ShowInfo();
	
	return true;
}

void BigHeadScreamers::RenderScene() const
{
	// Clear color and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//glDisable(GL_DEPTH_TEST);
	
	// Render skybox to framebuffer
	RenderSkyBox();

	//glEnable(GL_DEPTH_TEST);

	if (!bReflectionFlag)
	{
		// Sets modelview matrix to LoadMatrixNoXZ()
		RenderGround();
	}
	// sets modelview matrix to LoadMatrix()
	RenderFire();
	// Uses previous modelview matrix
	RenderSprites();
}

void BigHeadScreamers::MultMirror() const
{
	if (bReflectionFlag)
	{
		Matrix4 mirror;
		mirror[1][1] = -1.0f; // invert y axis
		glMultMatrixf(mirror.data());	
	}
}

void BigHeadScreamers::RenderReflection() const
{
	// Causes all rendered objects to be premultiplied by the mirror matrix
	bReflectionFlag = true;
	pReflectionFBO->BindBuffer();
	
	RenderScene();
	
	pReflectionFBO->UnbindBuffer();
	bReflectionFlag = false;
}


void BigHeadScreamers::SkyBoxRotate() const 
{
	glLoadIdentity();
	// first rotate beta angle to avoid spin
	glRotatef(fpsCamera.GetBeta(), 1.0, 0.0, 0.0);
	// then rotate alpha angle to orient
	glRotatef(fpsCamera.GetAlpha(), 0.0, 1.0, 0.0);

	// Anything more than diagonal(cube) * Near
	float scale = 10.0f * Settings::Instance().Near;
	glScalef(scale, scale, scale);
	
	MultMirror();
}


	
void BigHeadScreamers::RenderSkyBox() const
{
	glPushMatrix();
	
	SkyBoxRotate();	

	pSkyBoxManager->Render();

	glPopMatrix();
}



void BigHeadScreamers::RenderGround() const
{
	fpsCamera.LoadMatrixNoXZ();

	glBindTexture(GL_TEXTURE_2D, CurrentTexture());

	// Set reflection texture (the shader in Ground expects it)
	glActiveTexture(GL_TEXTURE1);
	pReflectionFBO->BindTexture();
	glActiveTexture(GL_TEXTURE0);

	pGround->Render(fpsCamera.GetPosition(), Settings::Instance().Far,
		ShellGet(SDLShell::SHELL_WIDTH), ShellGet(SDLShell::SHELL_HEIGHT));
}




void BigHeadScreamers::RenderFire() const
{
	//glPushMatrix();
	fpsCamera.LoadMatrix();
	MultMirror();

	pWM->Render();

	if (!bReflectionFlag)
		pExpR->Render(pAI->GetParticles());

	//glPopMatrix();
}

void BigHeadScreamers::RenderSprites() const
{
	//fpsCamera.LoadMatrix();
	//MultMirror();

	// Note alpha mask is needed since the polygons z-fight otherwise
	pER->Render(pAI->GetData(), -fpsCamera.GetAlpha(), Settings::Instance().EnemyHeight);
}


void BigHeadScreamers::DrawCoordinateFrame() const
{
	glClear(GL_DEPTH_BUFFER_BIT);
	
	float offset[] = { -0.9f, -0.9f };
	GLuint shader = Program(P_COLOR_OFFSET);
	glUseProgram(shader);
	glUniform2fv(GetUniLoc(shader, "Offset"), 1, offset);

	glPushMatrix();
	glLoadIdentity();

	// FIXME: define translation for coordinate frame
	glTranslatef(0.0f, 0.0f, -250);
	glRotatef(fpsCamera.GetBeta(), 1.0f, 0.0f, 0.0f);
	glRotatef(fpsCamera.GetAlpha(), 0.0f, 1.0f, 0.0f);

	CoordinateFrame::Instance().Render();
	glPopMatrix();
}


void BigHeadScreamers::RenderReflectionFBO() const
{
	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	glLoadIdentity();

	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glLoadIdentity();

	glUseProgram(Program(P_LOOKUP));

	pReflectionFBO->BindTexture();

	RenderQuad2D(0.68f, -0.98f, 0.3f, 0.3f, 0.0f, 0.0f, 1.0f, 1.0f);

	glPopMatrix();

	glMatrixMode( GL_MODELVIEW );
	glPopMatrix();
}


void BigHeadScreamers::ShowInfo()
{
	if (!iShowInfo)
		printf("FPS=%.1f\n", 1.0f / timer.GetDeltaTime());

	if (iShowInfo)
	{
		glDisable(GL_DEPTH_TEST);
		if (iShowInfo >= 2)
		{
			// Clears depth buffer
			DrawCoordinateFrame();			
			
			RenderReflectionFBO();

			fpsGraph.Draw();

		}

		/* Draw font */
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
		//glEnable(GL_TEXTURE_2D);

		glUseProgram(Program(P_LOOKUP));

		TTFont::glEnable2D();
		
		SDL_Color color;
		SDL_Rect position;

		color.r = 255;
		color.g = 255;
		color.b = 255;

		if (iShowInfo == 1)
		{
			position.x = (int)(ShellGet(SHELL_WIDTH) * 0.01f);
			position.y = (int)(ShellGet(SHELL_HEIGHT) * 0.95f);

			ttFont.SDL_GL_RenderText(color, &position, "%.1f",  1.0f / timer.GetDeltaTime());

			position.y -= position.h * 1.2f;
			ttFont.SDL_GL_RenderText(color, &position, "%.2fms", timer.GetDeltaTime() * 1000.0f);

			position.x = (int)(ShellGet(SHELL_WIDTH) * 0.85f);
			position.y = (int)(ShellGet(SHELL_HEIGHT) * 0.95f);

			ttFont.SDL_GL_RenderText(color, &position, "bullets=%d", pWM->GetBullets().size());

			position.y -= position.h * 1.2f;
			ttFont.SDL_GL_RenderText(color, &position, "input=%.2fms", fInputTime * 1000.0f);

			position.y -= position.h * 1.2f;
			ttFont.SDL_GL_RenderText(color, &position, "collisions[%s]=%.2fms", enabled[S_COLLISIONS] ? "E" : "D", fCollisionTime * 1000.0f);

			position.y -= position.h * 1.2f;
			ttFont.SDL_GL_RenderText(color, &position, "reflection=%s", enabled[S_REFLECTION] ? "yes" : "no");
		}
		else
		{
			position.x = (int)(ShellGet(SHELL_WIDTH) * 0.01f);
			position.y = (int)(ShellGet(SHELL_HEIGHT) * 0.75f);

			ttFont.SDL_GL_RenderText(color, &position, "FPS=%.1f",  1.0f / timer.GetDeltaTime());
			position.y -= position.h * 1.2f;
		
			ttFont.SDL_GL_RenderText(color, &position, "FOV=%.2f", fFOV);
			position.y -= position.h * 1.2f;

			ttFont.SDL_GL_RenderText(color, &position, "collisions=%.2fms", fCollisionTime * 1000.0f);
			position.y -= position.h * 1.2f;

			ttFont.SDL_GL_RenderText(color, &position, "enemies=%.2fms", fEnemiesTime * 1000.0f);
			position.y -= position.h * 1.2f;

			if (iShowInfo >= 3)
			{
				ttFont.SDL_GL_RenderText(color, &position, "alpha=%.1f", fpsCamera.GetAlpha());
				position.y -= position.h * 1.2f;

				ttFont.SDL_GL_RenderText(color, &position, "beta=%.1f", fpsCamera.GetBeta());
				position.y -= position.h * 1.2f;

				const Vector3 tr = fpsCamera.GetPosition();
				ttFont.SDL_GL_RenderText(color, &position, "pos=[%.1f,%.1f,%.1f]", tr[0], tr[1], tr[2]);
				position.y -= position.h * 1.2f;

				ttFont.SDL_GL_RenderText(color, &position, "#=%d", pGround->VisibleVertices());
				position.y -= position.h * 1.2f;

				for (int i = 0; i < pGround->VisibleVertices(); i++)
				{
					ttFont.SDL_GL_RenderText(color, &position, "w[%d]=[%.1f,%.1f,%.1f]", i, (*pGround)[i][0], (*pGround)[i][1], (*pGround)[i][2]);
					position.y -= position.h * 1.2f;
				}
			}
		}

		TTFont::glDisable2D();
		glDisable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_DEPTH_TEST);

	}
}


bool BigHeadScreamers::ReleaseGL()
{
	CoordinateFrame::Instance().Unload();

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


