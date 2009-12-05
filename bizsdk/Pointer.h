/*****************************************************************************
 * Filename			Pointer.h
 * 
 * License			LGPL
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
#include <SDL/SDL.h>
#include <deque>

using namespace std;

struct PointerInputSample
{
	float dx, dy, dt, t;
};

class Pointer
{
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
public:
	Pointer(Shell *shell);
	~Pointer();

	void Input();

	void UpdateMouseMotion(SDL_MouseMotionEvent &event);
	void UpdateMouseButton(SDL_MouseButtonEvent &event);

	// Get functions
	bool Pressed() { return !prevPressing && currPressing; }
	bool Pressing() { return currPressing; }
	bool Released() { return prevPressing && !currPressing; }

	float GetMotionX() { return motionX; }
	float GetMotionY() { return motionY; }
	float GetReleaseMotionX() { return releaseMotionX; }
	float GetReleaseMotionY() { return releaseMotionY; }
	float GetPositionX() { return positionX; }
	float GetPositionY() { return positionY; }
	float GetMotionTime() { return motionTime; }
	float GetInertiaX() { return inertiaX; }
	float GetInertiaY() { return inertiaY; }

	float TimeSinceLastInput() { return timeSinceLastInput; }
	float LastInputTime() { return motionTime > releaseTime ? motionTime : releaseTime; }
};


#endif

