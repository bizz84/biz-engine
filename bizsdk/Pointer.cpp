/*****************************************************************************
 * Filename			Pointer.cpp
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

#include "Pointer.h"
#include "Misc.h"
#include "SDLShell.h"


Pointer::Pointer(Shell *shell)
{
	pShell = shell;
	maxDelay = 0.2f;
	inertiaTau = 2.0f;

	prevPressing = false;
	currPressing = false;

	// motion, position and inertia
	motionX = 0.0f;
	motionY = 0.0f;
	releaseMotionX = 0.0f;
	releaseMotionY = 0.0f;
	positionX = 0.0f;
	positionY = 0.0f;
	inertiaX = 0.0f;
	inertiaY = 0.0f;

	// time
	motionTime = 0.0f;
	releaseTime = -10.0f;
	timeSinceLastInput = 0.0f;

	read = false;

	timer.Start();
}

Pointer::~Pointer()
{
	samples.clear();
}

void Pointer::Input()
{
	prevPressing = currPressing;
	read = false;

	timer.Update();

	RemoveOld();

	Update();

	float inertia = Inertia(timer.GetTime() - releaseTime, inertiaTau);
	inertiaX = releaseMotionX * inertia;
	inertiaY = releaseMotionY * inertia;

	timeSinceLastInput = timer.GetTime() - LastInputTime();
}

void Pointer::RemoveOld()
{
	// Remove old samples
	if (samples.size() > 0)
	{
		deque<PointerInputSample>::iterator last, first = samples.begin();

		for(last = samples.begin(); last != samples.end(); last++)
		{
			if (timer.GetTime() - last->t < maxDelay)
				break;
		}
		samples.erase(first, last);
	}
}

void Pointer::Update()
{
	motionX = 0.0f;
	motionY = 0.0f;
	if (samples.size() >= 1)
	{
		int count = 0;

		deque<PointerInputSample>::iterator iter;

		for(iter = samples.begin(); iter != samples.end(); iter++)
		{
			if (iter->dt > 0.0f)
			{
				count++;
				motionX += iter->dx / iter->dt;
				motionY += iter->dy / iter->dt;
			}
		}
		if (count)
		{
			motionX /= (float)count;
			motionY /= (float)count;
		}
	}
}

bool Pointer::MotionCondition()
{
	return currPressing && !read;
}

void Pointer::UpdateMouseMotion(SDL_MouseMotionEvent &event)
{
	if (MotionCondition())
	{
		read = true;
		timer.Update();
		float dt = samples.size() > 0 ?
			timer.GetTime() - samples.back().t : 0.0f;
		samples.push_back(GenerateSample(event, dt));
		/*if (Verbose(VerboseAll))
		{
			PointerInputSample sample = samples.back();
			printf("x=%.3f y=%.3f dx=%.3f dy=%.3f t=%.5f dt=%.5f\n",
				positionX, positionY, sample.dx
				sample.dy, sample.t, sample.dt);
		}*/
	}
}

void Pointer::UpdateMouseButton(SDL_MouseButtonEvent &event)
{
	if (event.button == 1)
	{
		currPressing = event.state == SDL_PRESSED;
		if (!currPressing)
		{
			timer.Update();
			releaseTime = timer.GetTime();
			releaseMotionX = motionX;
			releaseMotionY = motionY;
		}
	}
}

PointerInputSample Pointer::GenerateSample(SDL_MouseMotionEvent &event,
										   float dt)
{
	SDLShell *shell = (SDLShell *)pShell;
	PointerInputSample sample;

	positionX = 2.0f * (float)event.x /
		(float)shell->ShellGet(SDLShell::SHELL_WIDTH) - 1.0f;
	positionY = 1.0f - 2.0f * (float)event.y /
		(float)shell->ShellGet(SDLShell::SHELL_HEIGHT);

	sample.dx = (float)event.xrel;
	sample.dy = -(float)event.yrel;

	//sample.dx = 2.0f * (float)event.xrel / 
	//	(float)shell->ShellGet(SDLShell::SHELL_WIDTH);
	//sample.dy = -2.0f * (float)event.yrel /
	//	(float)shell->ShellGet(SDLShell::SHELL_HEIGHT);

	sample.t = motionTime = timer.GetTime();
	sample.dt = dt;

	return sample;
}

FPSPointer::FPSPointer(Shell *shell) : Pointer(shell)
{
	SDL_WM_GrabInput(SDL_GRAB_ON);
	SDL_ShowCursor(0);
}

bool FPSPointer::MotionCondition()
{
	return ((SDLShell *)pShell)->GetCurrentFrame() > 0;
}
