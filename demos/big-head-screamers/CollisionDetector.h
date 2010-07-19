/*****************************************************************************
 * Filename			CollisionDetector.h
 * 
 * License			LGPL
 *
 * Author			Andrea Bizzotto (bizz84@gmail.com)
 *
 * Platform			LinuxX11 / OpenGL
 * 
 * Description		Collision Detector
 *
 *****************************************************************************/
#ifndef _COLLISION_DETECTOR_H_
#define _COLLISION_DETECTOR_H_

#include "Extensions.h"

#include "Vector.h"
#include "Weapon.h"
#include "AIManager.h"

// Generic collision detector
class CollisionDetector
{
public:
	CollisionDetector();
	virtual ~CollisionDetector() { }

	// Implemented by derived classes to write the operand arrays before computation
	virtual void Write() = 0;
	// Actual processing
	virtual void Execute();
	// Implemented by derived classes to read results after computation
	virtual void Read() = 0;
	
	void Run();
};

class SegmentSphereCollisionDetector : public CollisionDetector
{
public:
	

};

// Note: might be more efficient to use interleaved on cpu and 
// non-interleaved on OpenCL -> allocate on final class
class GrenadeEnemyCollisionDetector : public SegmentSphereCollisionDetector
{
	GrenadeLauncher *pLauncher;
	AIManager *pAI;
protected:
	GrenadeLauncher *GetLauncher() { return pLauncher; }
	AIManager *GetAIManager() { return pAI; }
public:
	GrenadeEnemyCollisionDetector(GrenadeLauncher *launcher, AIManager *ai)
		: pLauncher(launcher), pAI(ai) { }
		
};

class CPUGrenadeEnemyCollisionDetector : public GrenadeEnemyCollisionDetector
{
	struct PosState
	{
		Vector3 prev;
		Vector3 curr;
		bool hit;
	} *pos;
	
	struct Target
	{
		Vector3 pos;
		bool hit;
	} *target;
public:
	CPUGrenadeEnemyCollisionDetector(GrenadeLauncher *launcher, AIManager *ai)
		: GrenadeEnemyCollisionDetector(launcher, ai), pos(NULL), target(NULL) { }
		
	virtual void Write();
	virtual void Execute();
	virtual void Read();
};

/*class OpenCLGrenadeEnemyCollisionDetector : GrenadeEnemyCollisionDetector
{
	Vector3 *pos1;
	Vector3 *pos2;
	bool *hitpos;
	Vector3 *target;
	bool *hittarget;
public:
	OpenCLGrenadeEnemyCollisionDetector(GrenadeLauncher *launcher, AIManager *ai)
		: GrenadeEnemyCollisionDetector(launcher, ai) { }

	virtual void Write();
	virtual void Execute();
	virtual void Read();
};*/

class CollisionDetectorFactory
{
public:
	static CollisionDetector *CreateCPU(GrenadeLauncher *launcher, AIManager *ai)
	{
		return new CPUGrenadeEnemyCollisionDetector(launcher, ai);
	}
	
};

#endif