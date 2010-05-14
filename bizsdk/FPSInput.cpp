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

#include "FPSInput.h"
#include "Extensions.h"
#include "Keys.h"

#include <math.h>

#define ANG 0.001F
#define RAD 57.295779513F

FPSInput::FPSInput()
{
	alpha = 0.0f;
	beta = 0.0f;

	translateX = 0.0f;
	translateY = 0.0f;
	translateZ = 0.0f;
}
void FPSInput::FPSInit(float translationFactor, float ty/* = 0.0f*/)
{
	fTranslationFactor = translationFactor;
	translateY = ty;
}

void FPSInput::FPSMove(SDLShell *shell, float dt)
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
	float matrix00 = cos(alpha / RAD);
	float matrix01 = sin(alpha / RAD);
	float matrix10 = -sin(alpha / RAD);
	float matrix11 = cos(alpha / RAD);
	translateZ += matrix00 * moveZ + matrix01 * moveX;
	translateX += matrix10 * moveZ + matrix11 * moveX;
	translateY += moveY;

	LoadFPSMatrix();
}

void FPSInput::LoadFPSMatrix()
{
	// load identity matrix
	glLoadIdentity();

	// first rotate beta angle to avoid spin
	glRotatef(beta, 1.0, 0.0, 0.0);
	// then rotate alpha angle to orient
	glRotatef(alpha, 0.0, 1.0, 0.0);
	// finally apply the translation
	glTranslatef(translateX, translateY, translateZ);
}
