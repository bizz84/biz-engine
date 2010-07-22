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
#include "ProgramArray.h"

// is-implemented-in-terms-of
class Ground : private ProgramArray
{
	unsigned int uiInfPlaneVertices;
	Vector3 vGround[5];

	enum { P_INFINITE, NUM_PROGRAMS };

	SDLShell *pShell;
public:
	Ground(SDLShell *shell);

	bool Init();

	// Conceptually Input() and Render() are distinct since one
	// updates the state, the other renders
	void Input(const Matrix4 &invProjView,
		const Vector3 &eyePos, const float zfar);

	void Render(const Vector3 &eyePos, const float zfar) const;

	const unsigned int VisibleVertices() const { return uiInfPlaneVertices; }

	Vector3 &operator[](int i) { return vGround[i]; }
	const Vector3 &operator[](int i) const { return vGround[i]; }
};

#endif

