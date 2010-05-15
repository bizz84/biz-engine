/*****************************************************************************
 * Filename			Timer.h
 * 
 * License			LGPL
 *
 * Author			Andrea Bizzotto (bizz84@gmail.com)
 *
 * Platform			LinuxX11 / OpenGL
 * 
 * Description		High resolution timer
 *
 *****************************************************************************/

#ifndef _TIMER_H_
#define _TIMER_H_

// Include header file containing the timeval struct declaration
#ifdef __linux__
#include <sys/time.h>
#else
#include <windows.h>
#endif

class Timer
{
	float fTimeCur, fTimePrev;
	struct timeval start;
	void Set(timeval &tv);
public:
	Timer();

	void Start();

	float Update();

	const float GetTime() const { return fTimeCur; }
	const float GetDeltaTime() const { return fTimeCur - fTimePrev; }
};

#endif

