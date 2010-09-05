/*****************************************************************************
 * Filename			BigHeadScreamers.cpp
 * 
 * License			GPLv3
 *
 * Author			Andrea Bizzotto (bizz84@gmail.com)
 *
 * Platform			LinuxX11 / OpenGL
 * 
 * Description		BigHeadScreamers
 *
 *****************************************************************************/

#include "BigHeadScreamers.h"
// Tools includes
#include "Keys.h"
#include "Misc.h"
#include "GLResourceManager.h"
#include "SkyBoxManager.h"
#include "TextGraph.h"
#include "FBO.h"

// App includes
#include "EnemyRendererAttrib.h"
#include "EnemyRendererBasic.h"
#include "Ground.h"
#include "AIManager.h"
#include "WeaponManager.h"
#include "ParticleRenderer.h"
#include "EnemyRendererAttrib.h"
#include "CollisionDetector.h"

#include "GrenadeRenderer.h"
#include "Settings.h"
#include "Fonts/FontGothic.h"
#include "Fonts/FontTechno.h"

#include <iostream>

// Shaders
static const char *Shaders[] = {
	"data/shaders/Lookup.vert", "data/shaders/Lookup.frag",
	"data/shaders/ColorOffset.vert", "data/shaders/ColorOffset.frag"
};

static const char *TimerStrings[] = {
	"Weapons",
	"AI",
	"Collisions",
	"Enemy Renderer",
	"Input",
};

BigHeadScreamers::BigHeadScreamers() : 
	iShowInfo(0),
	bReflectionFlag(false),
	fSetTime(0.0f),
	fRandomTime(0.0f),
	fFOV(90.0f),
	eCollisionType(0),
	uiNumComparisons(0)
{
	// initialize random number generator
	Timer::InitRand();

	bFeatureEnabled[F_REFLECTION] = true;
	bFeatureEnabled[F_INPUT] = true;

	for (unsigned int i = 0; i < NUM_TIMERS; i++)
		afTimeOf[i] = 0.0f;
}

bool BigHeadScreamers::InitApp()
{
	// Custom command line processing
	vector<CmdLineParameter>::const_iterator iter;
	const vector<CmdLineParameter> &params = GetCommandLineParams();

	for (iter = params.begin(); iter != params.end(); iter++)
	{
		if (iter->sName == "showinfo")
		{
			iShowInfo = atoi(iter->sValue.c_str());
		}
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
	// Initialize camera
	pFPSCamera = auto_ptr<FPSCamera>(new FPSCamera());
	pFPSCamera->Init(5.0f * 20.0f, -20.0f);

	// Initialize Ground
	pGround = auto_ptr<Ground>(new Ground());
	if (!pGround->Init())
		return false;

	// Load Shaders
	if (!LoadShaders(Shaders, NUM_PROGRAMS))
		return false;

	// Initialize skybox singleton
	pSkyBoxManager = auto_ptr<SkyBoxManager>(new SkyBoxManager());

	// Initialize coordinate frame. TODO: this class needs redefining
	// to get rid of deprecated GL calls
	CoordinateFrame::Instance().Make(400);

	// Initialize fbo used for drawing reflection
	//ReloadFBO(); // Commented out since it's called by Resize() later on

	// Initialize AI 
	pAI = auto_ptr<AIManager>(new AIManager(pFPSCamera->GetPosition()));

	// Initialize weapon system
	pWM = auto_ptr<WeaponManager>(new WeaponManager());

	// Initialize weapon renderer
	pPR = auto_ptr<ParticleRenderer>(new ParticleRenderer());

	// Initialize enemy renderer	
	pER = auto_ptr<EnemyRendererAttrib>(new EnemyRendererAttrib());

	// Enable two collision detectors (used to compare at runtime)
	pDetector[DETECTOR_SEGMENT_SPHERE] = 
		auto_ptr<CollisionDetector>(new CPUSegmentSphereCollisionDetector(pWM.get(), pAI.get()));
	pDetector[DETECTOR_SPHERE_SPHERE] = 
		auto_ptr<CollisionDetector>(new CPUSphereSphereCollisionDetector(pWM.get(), pAI.get()));
	pDetector[DETECTOR_QUAD_TREE] = 
		auto_ptr<CollisionDetector>(new QuadTreeCollisionDetector(pWM.get(), pAI.get()));
	pDetector[DETECTOR_NULL] = 
		auto_ptr<CollisionDetector>(new NullCollisionDetector(pWM.get(), pAI.get()));

	pFont = auto_ptr<FontManager>(new FontGothic("data/textures/font_gray.bmp"));
	//pFont = auto_ptr<FontManager>(new FontTechno("data/textures/FontB.bmp"));
	if (!pFont->Init())
		return false;

	// Initialize FPS graph
	pFPSGraph = auto_ptr<TextGraph>(new TextGraph());
	//if (!pFPSGraph->Init(true, true, true, true, "%.1f", 0.075f, 4.0f, 3000, 0.6f, 0.55f, 0.98f, 0.90f, false))
	if (!pFPSGraph->Init(true, true, true, true, "%.1f", 0.08f, 4.0f, 3000, 0.45f, 0.55f, 0.98f, 0.90f, false))
		return false;


	/* GL state setup */

	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );

	// Used by particle renderer
	glPointSize(Settings::Instance().PointSize);

	// Loads default value (FOV can be changed by user)
	fFOV = Settings::Instance().Fov;

	// Sets projection and modelview matrices, loads reflection FBO
	Resize(ShellGet(SHELL_WIDTH), ShellGet(SHELL_HEIGHT));

	// These operations are performed in Input() but are set here to update the state
	// in case Input() is not used (benchmarking)
	pFPSCamera->Update(this, 0.0f);
	pER->Update(pAI->GetData(), -pFPSCamera->GetAlpha(), Settings::Instance().EnemyHeight);
	GroundInput();

	// This is for GL state variables that won't change across the whole program
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glEnableClientState(GL_VERTEX_ARRAY);	
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	timer.Start();
	return true;
}

void BigHeadScreamers::SetupProjection(unsigned int width, unsigned int height)
{
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
	// Select modelview matrix
	glMatrixMode( GL_MODELVIEW );
}


bool BigHeadScreamers::Resize(unsigned int width, unsigned int height)
{
	SetupProjection(width, height);
	ReloadFBO();
	return true;
}

void BigHeadScreamers::GroundInput()
{
	// Ground input
	Matrix4 rot = AlphaBetaRotation(pFPSCamera->GetAlpha() * M_PI / 180.0f, pFPSCamera->GetBeta() * M_PI / 180.0f);
	Vector3 translationNoXZ = Vector3(0.0f, pFPSCamera->GetTranslation()[1], 0.0f);
	Matrix4 projViewInv = InverseMVP(mInvProj, translationNoXZ, rot);
	pGround->Input(projViewInv, pFPSCamera->GetPosition(), Settings::Instance().Far);
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
		pGround->PrevTexture();
	if (KeyPressed(KEY_2))
		pGround->NextTexture();

	if (KeyPressed(KEY_3))
		pSkyBoxManager->CubemapUpdate(false, t);
	if (KeyPressed(KEY_4))
		pSkyBoxManager->CubemapUpdate(true, t);

	// Update FOV
	if (KeyPressing(KEY_5))
	{
		if ((fFOV /= (1.0f + 0.25 * dt)) < 40.0f)
			fFOV = 40.0f;
		SetupProjection(ShellGet(SHELL_WIDTH), ShellGet(SHELL_HEIGHT));
	}
	if (KeyPressing(KEY_6))
	{
		if ((fFOV *= (1.0f + 0.25 * dt)) > 150.0f)
			fFOV = 150.0f;
		SetupProjection(ShellGet(SHELL_WIDTH), ShellGet(SHELL_HEIGHT));
	}
	// Restore original fov
	if (KeyPressed(KEY_7))
	{
		fFOV = Settings::Instance().Fov;
		SetupProjection(ShellGet(SHELL_WIDTH), ShellGet(SHELL_HEIGHT));
	}
	if (KeyPressed(KEY_8))
	{
		eCollisionType = Next(eCollisionType, NUM_DETECTORS);
		//bFeatureEnabled[F_COLLISIONS] = !bFeatureEnabled[F_COLLISIONS];
	}
	if (KeyPressed(KEY_9))
	{
		bFeatureEnabled[F_REFLECTION] = !bFeatureEnabled[F_REFLECTION];
	}
	if (KeyPressed(KEY_0))
	{
		bFeatureEnabled[F_INPUT] = !bFeatureEnabled[F_INPUT];
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
	pFPSCamera->Update(this, dt);

	GroundInput();

	
	if (RightClick())
		GetPointer()->ToggleLock();

	if (bFeatureEnabled[F_INPUT])
	{
		// Loop through cubemaps
		if (Settings::Instance().MinRandomCycle != 0.0f && t - fSetTime > fRandomTime)
		{
			fSetTime = timer.GetTime();
			fRandomTime = RandRange(Settings::Instance().MinRandomCycle, Settings::Instance().MaxRandomCycle);
			pSkyBoxManager->CubemapUpdate(true, t);
		}
		pSkyBoxManager->Update(t);

		// Game input
		Timer timer;
		// Weapons input
		timer.Start();
		pWM->Input(dt, *pFPSCamera, LeftClick() || KeyPressing(KEY_SPACE));
		afTimeOf[TIME_WEAPON] = timer.Update();		
		
		// AI input
		timer.Start();
		pAI->Input(t, dt, pFPSCamera->GetPosition());
		afTimeOf[TIME_AI] = timer.Update();

		// Collisions
		timer.Start();
		uiNumComparisons = pDetector[eCollisionType]->Run();
		afTimeOf[TIME_COLLISIONS] = timer.Update();

		// Weapons update
		timer.Start();
		pWM->UpdateState();
		afTimeOf[TIME_WEAPON] += timer.Update();

		// AI update
		timer.Start();
		pAI->UpdateState(pFPSCamera->GetPosition());
		afTimeOf[TIME_AI] += timer.Update();

		// Enemy Renderer update
		timer.Start();
		pER->Update(pAI->GetData(), -pFPSCamera->GetAlpha(), Settings::Instance().EnemyHeight);
		afTimeOf[TIME_ENEMY_RENDERER] = timer.Update();

	}
	// Overlay update
	if (iShowInfo >= 1)
	{
		// Graph input
		pFPSGraph->Input(1.0f / dt, t);
	}

	afTimeOf[TIME_INPUT] = inputTime.Update();
}


bool BigHeadScreamers::Render()
{
	Input();	
	
	/* Rendering */
	// First render mirrored content to FBO
	if (bFeatureEnabled[F_REFLECTION])
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
		
	// Render skybox to framebuffer
	RenderSkyBox();

	if (!bReflectionFlag)
	{
		// Sets modelview matrix to LoadMatrixNoXZ()
		RenderGround();
	}
	// sets modelview matrix to LoadMatrix()
	RenderContent();
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
	glRotatef(pFPSCamera->GetBeta(), 1.0, 0.0, 0.0);
	// then rotate alpha angle to orient
	glRotatef(pFPSCamera->GetAlpha(), 0.0, 1.0, 0.0);

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
	pFPSCamera->LoadMatrixNoXZ();

	// Set reflection texture (the shader in Ground expects it)
	glActiveTexture(GL_TEXTURE1);
	pReflectionFBO->BindTexture();
	glActiveTexture(GL_TEXTURE0);

	pGround->Render(pFPSCamera->GetPosition(), Settings::Instance().Far,
		ShellGet(SDLShell::SHELL_WIDTH), ShellGet(SDLShell::SHELL_HEIGHT));
}

void BigHeadScreamers::RenderContent() const
{
	pFPSCamera->LoadMatrix();
	MultMirror();

	glEnable(GL_BLEND);
	// Note alpha mask is needed since the polygons z-fight otherwise
	pER->Render(pAI->GetData(), -pFPSCamera->GetAlpha(), Settings::Instance().EnemyHeight);
	glDisable(GL_BLEND);

	pWM->Render();

	if (!bReflectionFlag)
		pPR->Render(pAI->GetParticles());
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
	glRotatef(pFPSCamera->GetBeta(), 1.0f, 0.0f, 0.0f);
	glRotatef(pFPSCamera->GetAlpha(), 0.0f, 1.0f, 0.0f);

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


void BigHeadScreamers::ShowInfo() const
{
	if (!iShowInfo)
		printf("FPS=%.1f\n", 1.0f / timer.GetDeltaTime());

	if (iShowInfo)
	{
		if (iShowInfo >= 2)
		{
			// Clears depth buffer
			DrawCoordinateFrame();
		}

		glDisable(GL_DEPTH_TEST);
		
		if (iShowInfo >= 2)
		{			
			RenderReflectionFBO();
		}

		pFPSGraph->Draw();

		glEnable(GL_BLEND);
		pFont->Bind();

		ShowIntro(timer.GetTime());

		pFPSGraph->TextDraw(pFont.get());

		if (iShowInfo >= 2)
		{
			// font parameters
			float color[] = {1.0,1.0,0.0,1.0};
			float scale = 0.06f;
			float mscale = 0.07f;
			float x = -1.0f;
			float y = 1.0f + scale;

			FontManager::HorzAlign horz = FontManager::LeftAlign;
			FontManager::VertAlign vert = FontManager::TopAlign;


			pFont->Render(x, y -= mscale, scale, color, horz, vert,
				"B=%d,E=%d", pWM->GetBullets().size(), pAI->GetData().size());

			pFont->Render(x, y -= mscale, scale, color, horz, vert,
				"Detector=%d, comp=%d", eCollisionType, uiNumComparisons);

			for (unsigned int i = 0; i < NUM_TIMERS; i++)
			{
				y -= mscale;
				pFont->Render(x, y, scale, color, horz, vert,
					"%.2fms", afTimeOf[i] * 1000.0f);
				pFont->Render(-0.80f, y, scale, color, horz, vert,
					"%s", TimerStrings[i]);
			}
			pFont->Render(x, y -= mscale, scale, color, horz, vert,
				"%.2fms", timer.GetDeltaTime() * 1000.0f);

			//pFont->Render(x, y -= mscale, scale, color, horz, vert,
			//	"E * B = %d * %d = %d", pAI->GetData().size(), pWM->GetBullets().size(),
			//	pAI->GetData().size() * pWM->GetBullets().size());

			pFont->Render(x, y -= mscale, scale, color, horz, vert,
				"FOV=%.2f", fFOV);

			// Uncomment this to see all available characters on scren
			//pFont->TestFont();
		}

		glDisable(GL_BLEND);

		glEnable(GL_DEPTH_TEST);
	}
}

void BigHeadScreamers::ShowIntro(float t) const
{
	const float titleTime = 3.0f;
	if (t < titleTime)
	{
		float c = (titleTime - t) / titleTime;
		float red[] = {1.0,0.0,0.0,c};
		float yellow[] = {1.0,1.0,0.0,c};
		
		pFont->Render(0.0,  0.1f, 0.15f, red, FontManager::CenterAlign, FontManager::MiddleAlign, GetAppName());
		pFont->Render(0.0, -0.1f, 0.15f, yellow, FontManager::CenterAlign, FontManager::MiddleAlign, GetAppVersion());
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
