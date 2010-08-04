/*****************************************************************************
 * Filename			Pointer.h
 * 
 * License			GPLv3
 *
 * Author			Andrea Bizzotto (bizz84@gmail.com)
 *
 * Platform			LinuxX11 / OpenGL
 * 
 * Description		Pointer class for mouse interface - position and motion
 * 					estimation
 *
 *****************************************************************************/

#ifndef _POINTER_H_
#define _POINTER_H_

#include "Timer.h"
#include "Shell.h"
#ifdef __linux__
#include <SDL/SDL.h>
#else
#include <SDL.h>
#endif
#include <deque>

using namespace std;

struct PointerInputSample
{
	float dx, dy, dt, t;
};

class Pointer
{
protected:
	float maxDelay;
	float inertiaTau;
	//! Only one update per frame is allowed
	bool read;

	deque<PointerInputSample> samples;

	bool prevPressing;
	bool currPressing;

	float motionX;
	float motionY;
	float releaseMotionX;
	float releaseMotionY;
	float positionX;
	float positionY;
	float motionTime;

	float releaseTime;
	float inertiaX;
	float inertiaY;

	float timeSinceLastInput;

	Timer timer;
	Shell *pShell;

	void RemoveOld();
	void Update();
	PointerInputSample GenerateSample(SDL_MouseMotionEvent &event, float dt);

	virtual const bool MotionCondition() const { return currPressing && !read; }
public:
	Pointer(Shell *shell);
	virtual ~Pointer();

	void Input();

	void UpdateMouseMotion(SDL_MouseMotionEvent &event);
	void UpdateMouseButton(SDL_MouseButtonEvent &event);

	// Get functions
	const bool Pressed() const { return !prevPressing && currPressing; }
	const bool Pressing() const { return currPressing; }
	const bool Released() const { return prevPressing && !currPressing; }

	const float GetMotionX() const { return motionX; }
	const float GetMotionY() const { return motionY; }
	const float GetReleaseMotionX() const { return releaseMotionX; }
	const float GetReleaseMotionY() const { return releaseMotionY; }
	const float GetPositionX() const { return positionX; }
	const float GetPositionY() const { return positionY; }
	const float GetMotionTime() const { return motionTime; }
	const float GetInertiaX() const { return inertiaX; }
	const float GetInertiaY() const { return inertiaY; }

	const float TimeSinceLastInput() const { return timeSinceLastInput; }
	const float LastInputTime() const { return motionTime > releaseTime ? motionTime : releaseTime; }

	const size_t NumSamples() const { return samples.size(); }

	virtual bool ToggleLock() { return false; }
	virtual bool GetLock() const { return false; }

	static const float MaxMotion;
};

class FPSPointer : public Pointer
{
	bool bMouseLock;
protected:
	virtual const bool MotionCondition() const;
public:
	FPSPointer(Shell *shell);
	virtual bool ToggleLock();
	virtual bool GetLock() const { return bMouseLock; }

};


#endif

