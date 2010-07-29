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

#include "SDLShell.h"
#include "ProgramArray.h"

#include "Timer.h"
#include "Matrix.h"
#include "CoordinateFrame.h"

using namespace std; // for auto_ptr

class FBO;
class Ground;
class AIManager;
class WeaponManager;
class ParticleRenderer;
class BulletRenderer;
class EnemyRenderer;
class CollisionDetector;
class SkyBoxManager;
class FPSCamera;
class TextGraph;
class FontManager;

// is-a SDLShell, is-implemented-in-terms-of ProgramArray
class BigHeadScreamers : public SDLShell, private ProgramArray
{
	enum {
		P_LOOKUP,		// used for font
		P_COLOR_OFFSET, // used for coordinate frame
		NUM_PROGRAMS 
	};	

	enum { F_COLLISIONS, F_REFLECTION, F_INPUT, NUM_FEATURES };
	bool bFeatureEnabled[NUM_FEATURES];

	int iShowInfo;

	// mutable since it's changed by RenderReflection() but restored at the end
	// aka logical constness
	mutable bool bReflectionFlag;
	auto_ptr<FBO> pReflectionFBO;

	// Timing related variables
	Timer timer;
	float fSetTime;
	float fRandomTime;

	// Timers used to measure input stages
	enum { TIME_INPUT, TIME_WEAPON, TIME_AI, TIME_COLLISIONS,
		TIME_ENEMY_RENDERER, NUM_TIMERS };
	float afTimeOf[NUM_TIMERS];

	// Projection matrix related variables
	float fFOV;
	// Inverted projection matrix (needed by infinite plane rendering)
	Matrix4 mInvProj;
	
	// Ground renderer
	auto_ptr<Ground> pGround;
	// Handles generation and rendering of bullets
	auto_ptr<WeaponManager> pWM;
	// Particle renderer, used to draw blood particles
	auto_ptr<ParticleRenderer> pPR;
	// AI Manager, handles enemies
	auto_ptr<AIManager> pAI;
	// Base class for enemy renderer
	auto_ptr<EnemyRenderer> pER;
	// Contains cubemaps and skybox load, update and rendering code
	auto_ptr<SkyBoxManager> pSkyBoxManager;
	// Collision detectors
	enum { DETECTOR_SEGMENT_SPHERE,	DETECTOR_SPHERE_SPHERE, NUM_DETECTORS };
	auto_ptr<CollisionDetector> pDetector[NUM_DETECTORS];

	// Game camera
	auto_ptr<FPSCamera> pFPSCamera;

	// Utility
	auto_ptr<TextGraph> pFPSGraph;
	auto_ptr<FontManager> pFont;

protected:
	// Overrides SDLShell version
	virtual Pointer *NewPointer() { return new FPSPointer(this); }

	// Updates matrices and ground state
	void GroundInput();

	// Loads reflection FBO. Actually reload since this is called
	// each time the window is resized
	void ReloadFBO();

	// Sets up projection matrix. Called by Resize() and fov update ops.
	void SetupProjection(unsigned int width, unsigned int height);

	// All input is processed here (called by Render())
	// TODO implement into SDLShell as non-const, make Render() const
	void Input();
	
	// Auxiliary methods to Render functions
	void SkyBoxRotate() const;
	void MultMirror() const;

	// Rendering methods
	void RenderReflection() const;
	void RenderScene() const;
	void RenderSkyBox() const;
	void RenderGround() const;
	void RenderContent() const;
	void DrawCoordinateFrame() const;
	void RenderReflectionFBO() const;
	void ShowInfo(); // TODO const
	
	// Virtuals
	virtual bool InitApp();
	virtual bool InitGL();
	virtual bool Render(); // TODO: const once Input is moved out
	virtual bool ReleaseGL();
	virtual bool ReleaseApp();	
	// Sets projection and modelview matrices, loads reflection FBO
	virtual bool Resize(unsigned int width, unsigned int height);

	virtual bool RequiresOpenGL2() { return true; }
	virtual bool RequiresTTF()  { return true; }

	virtual const char *GetAppName() { return szAppName; }
	virtual const char *GetAppVersion()  { return szAppVersion; }

public:
	BigHeadScreamers();
};

#endif

