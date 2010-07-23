/*****************************************************************************
 * Filename			NBody.h
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
#ifndef _UNITTEST_H_
#define _UNITTEST_H_

#include "Extensions.h"
#include "Matrix.h"

class DebugVector3 : public Vector3
{
public:
	void Print(const char *const fmt);
};

class DebugMatrix3 : public Matrix3
{
public:
	DebugMatrix3() { }
	DebugMatrix3(const float *f) : Matrix3(f) { }

	void Print(const char *const fmt);
	static void GLPrint(const float *f, const char *const fmt);
};

class DebugMatrix4 : public Matrix4
{
public:
	DebugMatrix4() { }
	DebugMatrix4(const float *f) : Matrix4(f) { }

	static void GLPrint(const float *f, const char *const fmt);
	static Vector4 GLMult(const float *f, const Vector4 &v);
};


void UnitTestRun();


bool AttribTest();


#endif