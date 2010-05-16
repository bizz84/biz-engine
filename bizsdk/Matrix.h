/*****************************************************************************
 * Filename			Matrix.h
 * 
 * License			LGPL
 *
 * Author			Andrea Bizzotto (bizz84@gmail.com)
 *
 * Platform			LinuxX11 / OpenGL
 * 
 * Description		Matrix operations
 *
 *****************************************************************************/
#ifndef _MATRIX_H_
#define _MATRIX_H_

#include "Extensions.h"
#include "Vector.h"

#include <iostream>

using namespace std;

/*****************************************************************************
 * Matrix3 class definition
 *****************************************************************************/

class Matrix3
{
protected:
	float s[9];
public:
	Matrix3();
	Matrix3(const float *f);

	static Matrix3 Identity() { return Matrix3(); }
	static const Matrix3 RotationX(const float angle);
	static const Matrix3 RotationY(const float angle);
	static const Matrix3 RotationZ(const float angle);

	const Vector3 operator*(const Vector3 &v) const;
	const Matrix3 operator*(const Matrix3 &m) const;

	//! Overloaded [] to return pointer to start of row i
	virtual float *operator[] (int i) { return s + 3 * i; }
	virtual const float *operator[] (int i) const { return s + 3 * i; }
	
	friend ostream &operator<<(ostream &stream, const Matrix3 &m);
};

/*****************************************************************************
 * Matrix4 class definition
 *****************************************************************************/

class Matrix4
{
protected:
	float s[16];
public:
	Matrix4();
	Matrix4(const float *f);

	static Matrix4 Identity() { return Matrix4(); }

	//Vector4 operator*(const Vector4 &v);

	//! Overloaded [] to return pointer to start of row i
	virtual float *operator[] (int i) { return s + 4 * i; }
	virtual const float *operator[] (int i) const { return s + 4 * i; }

	friend ostream &operator<<(ostream &stream, const Matrix4 &m);
};


#endif


