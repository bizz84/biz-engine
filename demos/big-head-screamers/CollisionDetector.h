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

class WeaponManager;
class AIManager;

// Generic collision detector
class CollisionDetector
{
	WeaponManager *pWM;
	AIManager *pAI;
protected:
	// Implemented by derived classes to write the operand arrays before computation
	virtual bool Write() = 0;
	// Actual processing
	virtual void Execute() = 0;
	// Implemented by derived classes to read results after computation
	virtual bool Read() = 0;

	WeaponManager *GetWM() { return pWM; }
	AIManager *GetAI() { return pAI; }	
public:
	CollisionDetector(WeaponManager *ws, AIManager *ai) : pWM(ws), pAI(ai) { }
	virtual ~CollisionDetector() { }
	
	void Run();
};

/*class SegmentSphereCollisionDetector : public CollisionDetector
{
public:
	

};*/

// Note: might be more efficient to use interleaved on cpu and 
// non-interleaved on OpenCL -> allocate on final class
/*class GrenadeEnemyCollisionDetector : public SegmentSphereCollisionDetector
{

protected:


public:
		
};*/

class CPUGrenadeEnemyCollisionDetector : public CollisionDetector//GrenadeEnemyCollisionDetector
{
	/*struct BulletState
	{
		Vector3 prev;
		Vector3 curr;
		bool hit;
	} *bullet;
	
	struct EnemyState
	{
		Vector3 pos;
		int health;
	} *enemy;*/
	
	//unsigned int uiNumColliders;
	//unsigned int uiNumTargets;
protected:
	virtual bool Write();
	virtual void Execute();
	virtual bool Read();

	//void SetNumTargets(const unsigned int targets) { uiNumTargets = targets; }
	//void SetNumColliders(const unsigned int colliders) { uiNumColliders = colliders; }
	//unsigned int GetNumColliders() const { return uiNumColliders; }
	//unsigned int GetNumTargets() const { return uiNumTargets; }

public:
	CPUGrenadeEnemyCollisionDetector(WeaponManager *ws, AIManager *ai)
		: CollisionDetector(ws, ai)
		//bullet(NULL), enemy(NULL),
		//uiNumColliders(0), uiNumTargets(0)
		{ }
		
};

/*class OpenCLGrenadeEnemyCollisionDetector : GrenadeEnemyCollisionDetector
{
	Vector3 *pos1;
	Vector3 *pos2;
	bool *hitpos;
	Vector3 *target;
	bool *hittarget;
protected:
	virtual bool Write();
	virtual void Execute();
	virtual bool Read();
public:
	OpenCLGrenadeEnemyCollisionDetector(WeaponManager *ws, AIManager *ai)
		: GrenadeEnemyCollisionDetector(ws, ws) { }

};*/

class CollisionDetectorFactory
{
public:
	static CollisionDetector *CreateCPU(WeaponManager *ws, AIManager *ai)
	{
		return new CPUGrenadeEnemyCollisionDetector(ws, ai);
	}
	
};

#endif