/*****************************************************************************
 * Filename			NBody.cpp
 * 
 * License			LGPL
 *
 * Author			Andrea Bizzotto (bizz84@gmail.com)
 *
 * Platform			LinuxX11 / OpenGL
 * 
 * Description		NBody Simulation
 *
 *****************************************************************************/

#include "CameraController.h"
#include "Extensions.h"
#include "Keys.h"

#define _USE_MATH_DEFINES
#include <math.h>

const float ANG = 0.001f;
#define M_RAD (180.0 / M_PI)
#define M_1_RAD (M_PI / 180.0)

FPSCamera::FPSCamera()
{
	alpha = 0.0f;
	beta = 0.0f;
}
void FPSCamera::Init(float translationFactor, float ty/* = 0.0f*/)
{
	fTranslationFactor = translationFactor;
	translation[1] = ty;
}

void FPSCamera::Update(SDLShell *shell, float dt)
{
	float moveX = 0.0, moveY = 0.0f, moveZ = 0.0f;
	
	// alpha rotations
	alpha += shell->GetPointer()->GetMotionX() * ANG;
	if (alpha >= 360.0f)
		alpha -= 360.0f;
	else if (alpha <= -360.0f)
	   alpha += 360.0f;

	// beta rotations
	beta -= shell->GetPointer()->GetMotionY() * ANG;
	if (beta > 80.0)
		beta = 80.0;
	else if (beta < -80.0)
		beta = -80.0;

	// Movement
	if (shell->KeyPressing(KEY_A))
		moveX += fTranslationFactor * dt;

	if (shell->KeyPressing(KEY_D))
		moveX -= fTranslationFactor * dt;

	if (shell->KeyPressing(KEY_Q))
		moveY -= fTranslationFactor * dt;

	if (shell->KeyPressing(KEY_E))
		moveY += fTranslationFactor * dt;

	if (shell->KeyPressing(KEY_W))
		moveZ += fTranslationFactor * dt;

	if (shell->KeyPressing(KEY_S))
		moveZ -= fTranslationFactor * dt;


	// translation parameters procedure
	float matrix00 = cos(alpha * M_1_RAD);
	float matrix01 = sin(alpha * M_1_RAD);
	float matrix10 = -sin(alpha * M_1_RAD);
	float matrix11 = cos(alpha * M_1_RAD);
	translation.Translate(
		matrix10 * moveZ + matrix11 * moveX,
		moveY,
		matrix00 * moveZ + matrix01 * moveX);
}

void FPSCamera::MultMatrix() const
{
	// first rotate beta angle to avoid spin
	glRotatef(beta, 1.0, 0.0, 0.0);
	// then rotate alpha angle to orient
	glRotatef(alpha, 0.0, 1.0, 0.0);
	// finally apply the translation
	glTranslatef(translation.s[0], translation.s[1], translation.s[2]);
}

void FPSCamera::LoadMatrix() const
{
	// load identity matrix
	glLoadIdentity();

	MultMatrix();
}

SpinCamera::SpinCamera()
{
	alpha = beta = 0.0f;
	radius = 1.0f;
}


void SpinCamera::Update(SDLShell *shell, float d)
{
	radius = d;

	alpha += 0.002f *
		(shell->GetPointer()->GetMotionX() +
		 shell->GetPointer()->GetInertiaX());
	beta -= 0.002f *
		(shell->GetPointer()->GetMotionY() +
		 shell->GetPointer()->GetInertiaY());

}

void SpinCamera::LoadMatrix()
{
	glLoadIdentity();

	MultMatrix();
}

void SpinCamera::MultMatrix()
{
	glTranslatef(0.0f, 0.0f, -radius);
	// first rotate beta angle to avoid spin
	glRotatef(beta, 1.0, 0.0, 0.0);
	// then rotate alpha angle to orient
	glRotatef(alpha, 0.0, 1.0, 0.0);
}

/*		glLoadIdentity();

		glTranslatef(0.0f, 0.0f, -fDistance);
		glRotatef(xRot, 1.0f, 0.0f, 0.0f);
		glRotatef(yRot, 0.0f, 1.0f, 0.0f);*/