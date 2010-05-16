/*****************************************************************************
 * Filename			NBody.h
 * 
 * License			LGPL
 *
 * Author			Andrea Bizzotto (bizz84@gmail.com)
 *
 * Platform			LinuxX11 / OpenGL
 * 
 * Description		First person shooter input
 *
 *****************************************************************************/
#ifndef _CAMERA_CONTROLLER_H_
#define _CAMERA_CONTROLLER_H_

#include "SDLShell.h"
#include "Matrix.h"

class CameraController
{

public:

	virtual void Update(SDLShell *shell, float) = 0;
	virtual void LoadMatrix() = 0;
	virtual void MultMatrix() = 0;

};

class FPSCamera : public CameraController
{
protected:
	// fps move stuff
	Vector3 translation;
	float alpha, beta;

	float fTranslationFactor;

public:
	FPSCamera();

	void Init(float translationFactor, float ty = 0.0f);

	virtual void Update(SDLShell *shell, float dt);
	virtual void LoadMatrix();
	virtual void MultMatrix();

	const float GetAlpha() const { return alpha; }
	const float GetBeta() const { return beta; }
	const Vector3 &GetTranslation() const { return translation; }
};

class SpinCamera : public CameraController
{
protected:
	float alpha, beta;
	float radius;
public:
	SpinCamera();

	virtual void Update(SDLShell *shell, float d);
	virtual void LoadMatrix();
	virtual void MultMatrix();
};

#endif