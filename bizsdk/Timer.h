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

#include <sys/time.h>

class Timer
{
	float fTimeCur, fTimePrev;
	struct timeval start;
	void Set(timeval &tv);
public:
	Timer();

	void Start();

	float Update();

	float GetTime();
	float GetDeltaTime();
};

#endif

