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
#ifndef _FPSINPUT_H_
#define _FPSINPUT_H_

#include "SDLShell.h"

class FPSInput
{
protected:
	// fps move stuff
	float translateX, translateY, translateZ;
	float alpha, beta;

	float fTranslationFactor;

public:
	FPSInput();

	void FPSInit(float translationFactor, float ty = 0.0f);
	void FPSMove(SDLShell *shell, float dt);
	void LoadFPSMatrix();
};

#endif