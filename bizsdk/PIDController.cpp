/*****************************************************************************
 * Filename			PIDController.cpp
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


#include "PIDController.h"

const float PIDController::TimeThreshold = 0.0005f;

PIDController::PIDController()
	: Kp(1.0f), Ki(0.0f), Kd(0.0f)
{
	Reset();
}

void PIDController::Reset()
{
	fError = 0.0f;
	fIntegral = 0.0f;
	fDerivative = 0.0f;
}

void PIDController::Init(float target, float kp, float ki, float kd)
{
	fTarget = target;
	Kp = kp;
	Ki = ki;
	Kd = kd;
}

// Pseudocode from: http://en.wikipedia.org/wiki/PID_controller
//previous_error = 0
//integral = 0 
//start:
//  error = setpoint - actual_position
//  integral = integral + (error*dt)
//  derivative = (error - previous_error)/dt
//  output = (Kp*error) + (Ki*integral) + (Kd*derivative)
//  previous_error = error
//  wait(dt)
//  goto start
float PIDController::Update(float dt, float target)
{
	if (dt < TimeThreshold)
		return 0.0f;
	float error = fTarget - target;
	fIntegral += error * dt;
	fDerivative = (error - fError) / dt;
	fError = error;
	return Kp * error + Ki * fIntegral + Kd * fDerivative;
}

void PIDController::UpdateParam(int index, float value)
{
	switch (index)
	{
	case -1:
		fTarget += value;
		break;
	case 0:
		Kp += value;
		break;
	case 1:
		Ki += value;
		break;
	case 2:
		Kd += value;
		break;
	}
}


PIDControllerGain::PIDControllerGain()
	: Kg(1.0f)
{

}

void PIDControllerGain::Init(float target, float kp, float ki, float kd, float kg)
{
	Kg = kg;
	PIDController::Init(target, kp, ki, kd);
}

float PIDControllerGain::Update(float dt, float target)
{
	return Kg * PIDController::Update(dt, target);
}

void PIDControllerGain::UpdateParam(int index, float value)
{
	if (index == 3)
	{
		Kg += value;
	}
	else
	{
		PIDController::UpdateParam(index, value);
	}
}

