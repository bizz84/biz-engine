/*****************************************************************************
 * Filename			Timer.cpp
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

#include "Timer.h"

#include <assert.h>
#include <stdlib.h>

// If linux is not defined, define gettimeofday using Windows high resolution
// timer
#ifndef __linux__
int gettimeofday(struct timeval *tp, void *tzp)
{
	LARGE_INTEGER frequency, time;
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&time);

	double t = (double)time.QuadPart /(double)frequency.QuadPart;

	tp->tv_sec = (time_t)t;
	tp->tv_usec = (time_t)(1000000.0 * (t - tp->tv_sec));

	return 0;
}
#endif

Timer::Timer() : fTimeCur(0.0f), fTimePrev(0.0f)
{
	Start();
}

void Timer::Start()
{
	Set(start);
}

void Timer::Set(timeval &tv)
{
	int ret = gettimeofday(&tv, NULL);
	assert(ret == 0);
}


float Timer::Update()
{
	struct timeval tv;
	Set(tv);
	
	fTimePrev = fTimeCur;
	fTimeCur = (float)(tv.tv_sec - start.tv_sec) +
		0.000001f * (float)(tv.tv_usec - start.tv_usec);
	return fTimeCur;
}
