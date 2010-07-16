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

#include "Vector.h"
#include "Matrix.h"

// TODO: Group functions by category

enum VerboseLevel { NoVerbose, VerboseInfo, VerboseAll };


VerboseLevel GetVerboseLevel();
void SetVerboseLevel(VerboseLevel level);
bool Verbose(VerboseLevel level);

#define MAX_PATH_LENGTH 256


int LoadFileIntoMemory(const char *filename, char **result);

void FreeFileMemory(char **memory);

bool LoadImage(const char *filename, SDL_Surface *&surface,
			   GLenum &textureFormat, GLint &nOfColors);

void RenderQuad2D(float x, float y, float width, float height,
				float u0, float v0, float u1, float v1);

int IntRound(double x);

int NextPowerOfTwo(int x);

float Inertia(float t, float tau);

void PrintOpenGLError();
GLint GetUniLoc(GLuint program, const GLchar *name);


float RandRange(float min, float max);

unsigned int InfinitePlane(Vector3 *poly, const float *plane, const Matrix4 &inv, const Vector3 &eye, const float zfar);
void IntersectionLinePlane(Vector3 &intersection, const float *plane, const Vector3 &a, const Vector3 &b);

// Note this will be column major as mandated by OpenGL
Matrix4 ProjectionRH(const float fov, const float invAspect, const float znear, const float zfar);
Matrix4 ProjectionRHInfinite(const float fov, const float invAspect, const float znear);
Matrix4 InverseProjectionRH(const float fov, const float aspect, const float znear, const float zfar);
Matrix4 InverseProjectionRHInfinite(const float fov, const float invAspect, const float znear);

Matrix4 AlphaBetaRotation(const float alpha, const float beta);
Matrix4 InverseMVP(const Matrix4 &invP, const Vector3 &T, const Matrix4 &R);

extern const int permutation[3];

#endif
