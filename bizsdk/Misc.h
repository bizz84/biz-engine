/*****************************************************************************
 * Filename			Misc.h
 * 
 * License			LGPL
 *
 * Author			Andrea Bizzotto (bizz84@gmail.com)
 *
 * Platform			LinuxX11 / OpenGL
 * 
 * Description		Utility functions
 *
 *****************************************************************************/

#ifndef MISC_H
#define MISC_H


#ifdef __linux__
#include <SDL/SDL.h>
#else
#include <SDL.h>
#endif
#include "Extensions.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include "Vector.h"
#include "Matrix.h"


/*****************************************************************************
 * Verbosity managemet
 *****************************************************************************/
enum VerboseLevel { NoVerbose, VerboseInfo, VerboseAll };
VerboseLevel GetVerboseLevel();
void SetVerboseLevel(VerboseLevel level);
bool Verbose(VerboseLevel level);


/*****************************************************************************
 * Loading files
 *****************************************************************************/
int LoadFileIntoMemory(const char *filename, char **result);

void FreeFileMemory(char **memory);

bool LoadImage(const char *filename, SDL_Surface *&surface,
			   GLenum &textureFormat, GLint &nOfColors);

/*****************************************************************************
 * Simple mathematical utilities
 *****************************************************************************/
int IntRound(double x);

inline unsigned int Next(unsigned int n, unsigned int mod)
{
	return n < mod - 1 ? n + 1 : 0;
}
inline unsigned int Prev(unsigned int n, unsigned int mod)
{
	return n > 0 ? n - 1 : mod - 1;
}

int NextPowerOfTwo(int x);

float Inertia(float t, float tau);

/*****************************************************************************
 * OpenGL related functions
 *****************************************************************************/
void PrintOpenGLError();
GLint GetUniLoc(GLuint program, const GLchar *name);
void RenderQuad2D(float x, float y, float width, float height,
				float u0, float v0, float u1, float v1);

/*****************************************************************************
 * Random functions
 *****************************************************************************/
float RandRange(float min, float max);
Point3 RandSphere();

/*****************************************************************************
 * Mathematical functions dealing planes and matrices
 *****************************************************************************/
// Infinite plane calculation
unsigned int InfinitePlane(Vector3 *poly, const float *plane,
	const Matrix4 &inv, const Vector3 &eye, const float zfar);
void IntersectionLinePlane(Vector3 &intersection, const float *plane,
	const Vector3 &a, const Vector3 &b);

// Note this will be column major as mandated by OpenGL
Matrix4 ProjectionRH(const float fov, const float invAspect,
					 const float znear, const float zfar);
Matrix4 ProjectionRHInfinite(const float fov, const float invAspect,
							 const float znear);
Matrix4 InverseProjectionRH(const float fov, const float aspect,
							const float znear, const float zfar);
Matrix4 InverseProjectionRHInfinite(const float fov, const float invAspect,
									const float znear);

Matrix3 AlphaBetaRotation(const float alpha, const float beta);
Matrix4 InverseMVP(const Matrix4 &invP, const Vector3 &T, const Matrix4 &R);

/*****************************************************************************
 * Collision detection routines
 *****************************************************************************/
bool CollisionSegmentSphere(const Vector3 &a, const Vector3 &b,
							const Vector3 &s, const float r);

#endif
