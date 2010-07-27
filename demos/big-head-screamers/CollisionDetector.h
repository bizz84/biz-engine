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

#include "Vector.h"

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

class CPUCollisionDetector : public CollisionDetector
{
	
protected:
	virtual bool Write() { return true; }
	virtual void Execute();
	virtual bool Read() { return true; }
	virtual bool Collision(const Vector3 &a, const Vector3 &b,
		const Vector3 &s, const float r) = 0;
public:
	CPUCollisionDetector(WeaponManager *ws, AIManager *ai)
		: CollisionDetector(ws, ai)
		{ }

};

class CPUSegmentSphereCollisionDetector : public CPUCollisionDetector
{
public:
	CPUSegmentSphereCollisionDetector(WeaponManager *ws, AIManager *ai)
		: CPUCollisionDetector(ws, ai)
		{ }
		
	virtual bool Collision(const Vector3 &a, const Vector3 &b,
		const Vector3 &s, const float r);
};

class CPUSphereSphereCollisionDetector : public CPUCollisionDetector
{
public:
	CPUSphereSphereCollisionDetector(WeaponManager *ws, AIManager *ai)
		: CPUCollisionDetector(ws, ai)
		{ }
		
	virtual bool Collision(const Vector3 &a, const Vector3 &b,
		const Vector3 &s, const float r);
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
		return new CPUSegmentSphereCollisionDetector(ws, ai);
	}
	
};

#endif