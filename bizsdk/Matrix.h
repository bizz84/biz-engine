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
	Matrix3(const float f00, const float f01, const float f02, 
	        const float f10, const float f11, const float f12, 
	        const float f20, const float f21, const float f22);

	static Matrix3 Identity() { return Matrix3(); }
	static const Matrix3 RotationX(const float angle);
	static const Matrix3 RotationY(const float angle);
	static const Matrix3 RotationZ(const float angle);

	const Vector3 operator*(const Vector3 &v) const;
	const Matrix3 operator*(const Matrix3 &m) const;

	//! Overloaded [] to return pointer to start of row i
	float *operator[] (int i) { return s + 3 * i; }
	const float *operator[] (int i) const { return s + 3 * i; }
	
	const float at(int i) const { return s[i]; }
	
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
	Matrix4(
		const float f00, const float f01, const float f02, const float f03,
	    const float f10, const float f11, const float f12, const float f13,  
	    const float f20, const float f21, const float f22, const float f23,  
	    const float f30, const float f31, const float f32, const float f33);
		
	Matrix4(const Matrix3 &mat3);

	static Matrix4 Identity() { return Matrix4(); }
	static Matrix4 Zero();
	static Matrix4 Translation(const Vector3 &t);

	//Vector4 operator*(const Vector4 &v);
	Matrix4 operator*(const Matrix4 &m) const;

	//! Overloaded [] to return pointer to start of row i
	float *operator[] (int i) { return s + 4 * i; }
	const float *operator[] (int i) const { return s + 4 * i; }
	//Matrix4 operator-() const;
	const Matrix4 operator-() const;


	const float at(int i) const { return s[i]; }

	friend ostream &operator<<(ostream &stream, const Matrix4 &m);
	// operations
	const Matrix4 Transpose() const ;
	const Matrix4 Inverse(const float epsilon) const;
	
	// access
	const float *data() const { return s; }
};


#endif


