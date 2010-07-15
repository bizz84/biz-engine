/*****************************************************************************
 * Filename			BigHeadScreamers.cpp
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
#include "SDLShell.h"
#include "Misc.h"

class Ground
{
	unsigned int uiInfPlaneVertices;
	Vector3 vGround[5];

	enum { 	E_LOOKUP, E_IDENTITY, E_INFINITE, NUM_SHADERS };
	GLuint uiProgram[NUM_SHADERS];

	void Projection(Vector4 &u, const float *proj, const float *mview, const Vector4 &v);
	void ZeroZ(Vector4 &w, const Vector4 &u);

public:
	Ground();

	bool Init();

	void Render(const Matrix4 &invProjView, const Vector3 &eyePos, const float zfar);

	const unsigned int VisibleVertices() const { return uiInfPlaneVertices; }

	Vector3 &operator[](int i) { return vGround[i]; }
	const Vector3 &operator[](int i) const { return vGround[i]; }
};

#endif

