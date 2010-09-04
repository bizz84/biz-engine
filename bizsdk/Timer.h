/*****************************************************************************
 * Filename			Timer.h
 * 
 * License			GPLv3
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

//! Class providing a high resolution cross platform timer
class Timer
{
	//! current time
	float fTimeCur;
	//! previous time
	float fTimePrev;
	//! Origin of time
	struct timeval start;
	//! Read current time into tv 
	static void Set(timeval &tv);
public:
	//! Constructor
	/*!
	 Starts the timer
	 */
	Timer();

	//! Starts the timer
	void Start();

	//! Updates the timer.
	float Update();

	//! Getter for time of last Update() invocation
	const float GetTime() const { return fTimeCur; }
	//! Difference between last and previous Update() invocations
	/*!
	 Note: Returns time between two consecutive frames if Update is called each
	 frame
	 */
	const float GetDeltaTime() const { return fTimeCur - fTimePrev; }

	//! Sets the random number generator with a random seed
	static void InitRand();
};

#endif

