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
#include "ProgramArray.h"

// is-implemented-in-terms-of
class Ground : private ProgramArray
{
	enum { NUM_TEXTURES = 2 };
	// Ground texture data
	unsigned int uiCurTexture;
	GLuint uiTexture[NUM_TEXTURES];

	unsigned int uiInfPlaneVertices;
	Vector3 vGround[5];

	enum { L_ZFAR, L_TEX_REPEAT, L_POS_OFFSET, L_SCREEN_INV, NUM_LOCATIONS };
	GLint loc[NUM_LOCATIONS];

	enum { P_INFINITE, NUM_PROGRAMS };

	bool LoadTextures();
public:
	Ground() : uiCurTexture(0), uiInfPlaneVertices(0) { }

	bool Init();

	// Conceptually Input() and Render() are distinct since one
	// updates the state, the other renders
	void Input(const Matrix4 &invProjView,
		const Vector3 &eyePos, const float zfar);

	void Render(const Vector3 &eyePos, const float zfar,
		const unsigned int width, const unsigned int height) const;

	const unsigned int VisibleVertices() const { return uiInfPlaneVertices; }

	Vector3 &operator[](int i) { return vGround[i]; }
	const Vector3 &operator[](int i) const { return vGround[i]; }

	// Texture methods
	const GLuint CurrentTexture() const { return uiTexture[uiCurTexture]; }
	void NextTexture() { uiCurTexture = Next(uiCurTexture, NUM_TEXTURES); }
	void PrevTexture() { uiCurTexture = Prev(uiCurTexture, NUM_TEXTURES); }
};

#endif

