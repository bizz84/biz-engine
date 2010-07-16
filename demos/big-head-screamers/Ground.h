/*****************************************************************************
 * Filename			Ground.h
 * 
 * License			LGPL
 *
 * Author			Andrea Bizzotto (bizz84@gmail.com)
 *
 * Platform			LinuxX11 / OpenGL
 * 
 * Description		Ground drawing functionality
 *
 *****************************************************************************/

#ifndef _GROUND_H_
#define _GROUND_H_


#include "Extensions.h"
#include "Vector.h"
#include "Matrix.h"
#include "SDLShell.h"

class Ground
{
	unsigned int uiInfPlaneVertices;
	Vector3 vGround[5];

	enum { 	E_INFINITE, NUM_SHADERS };
	GLuint uiProgram[NUM_SHADERS];

	SDLShell *pShell;
public:
	Ground(SDLShell *shell);

	bool Init();

	void Render(const Matrix4 &invProjView, const Matrix4 &view,
		const Vector3 &eyePos, const float zfar);

	const unsigned int VisibleVertices() const { return uiInfPlaneVertices; }

	Vector3 &operator[](int i) { return vGround[i]; }
	const Vector3 &operator[](int i) const { return vGround[i]; }
};

#endif

