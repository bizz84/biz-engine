/*****************************************************************************
 * Filename			PIDController.h
 * 
 * License			LGPL
 *
 * Author			Andrea Bizzotto (bizz84@gmail.com)
 *
 * Platform			LinuxX11 / OpenGL
 * 
 * Description		PID controller class
 *
 *****************************************************************************/


#ifndef _PID_CONTROLLER_H_
#define _PID_CONTROLLER_H_


class PIDController
{
protected:
	static const float TimeThreshold;

	float fError;
	float fIntegral;
	float fDerivative;
public:
	PIDController();

	void Init(float target, float kp, float ki, float kd);
	virtual float Update(float dt, float target);

	void Reset();

	virtual void UpdateParam(int index, float value);

	float GetError() { return fError; }
	float GetIntegral() { return fIntegral; }
	float GetDerivative() { return fDerivative; }
	
	float fTarget;
	float Kp;
	float Ki;
	float Kd;
};

//! PID Controller class with added gain parameter
/*!
	
*/
class PIDControllerGain : public PIDController
{
public:
	PIDControllerGain();

	void Init(float target, float kp, float ki, float kd, float kg);

	float Kg;

	virtual float Update(float dt, float target);

	virtual void UpdateParam(int index, float value);
};

#endif