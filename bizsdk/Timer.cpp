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


Timer::Timer() : fTimeCur(0.0f), fTimePrev(0.0f)
{

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
	fTimeCur = (float)(tv.tv_sec - start.tv_sec) + 0.000001f * (float)(tv.tv_usec - start.tv_usec);
	return fTimeCur;
}

float Timer::GetTime()
{
	return fTimeCur;
}

float Timer::GetDeltaTime()
{
	return fTimeCur - fTimePrev;
}

