/*****************************************************************************
 * Filename			Matrix.cpp
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

#include "Matrix.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>

/*****************************************************************************
 * Matrix3 implementation
 *****************************************************************************/

Matrix3::Matrix3()
{
	s[0] = 1.0f; s[1] = 0.0f; s[2] = 0.0f;
	s[3] = 0.0f; s[4] = 1.0f; s[5] = 0.0f;
	s[6] = 0.0f; s[7] = 1.0f; s[8] = 0.0f;
}
Matrix3::Matrix3(const float *f)
{
	float *p = s;
	for (unsigned int i = 0; i < 9; i++)
		*p++ = *f++;
}

const Matrix3 Matrix3::RotationX(const float angle)
{
	float c = cos(angle);
	float s = sin(angle);
	const float f[] = {
		1.0f, 0.0f, 0.0f,
		0.0f,    c,   -s,
		0.0f,    s,    c
	};
	return Matrix3(f);
}

const Matrix3 Matrix3::RotationY(const float angle)
{
	float c = cos(angle);
	float s = sin(angle);
	const float f[] = {
		   c, 0.0f,    s,
		0.0f, 1.0f, 0.0f,
		  -s, 0.0f,    c
	};
	return Matrix3(f);
}

const Matrix3 Matrix3::RotationZ(const float angle)
{
	float c = cos(angle);
	float s = sin(angle);
	const float f[] = {
		   c,   -s, 0.0f,
		   s,    c, 0.0f,
		0.0f, 0.0f, 1.0f
	};
	return Matrix3(f);
}

const Vector3 Matrix3::operator*(const Vector3 &v) const
{
	return Vector3(
		Vector3(s+0).dot(v),
		Vector3(s+3).dot(v),
		Vector3(s+6).dot(v));
}

const Matrix3 Matrix3::operator*(const Matrix3 &m) const
{
	Vector3 x(s+0), y(s+3), z(s+6);
	Vector3 mx(m.s[0], m.s[3], m.s[6]);
	Vector3 my(m.s[1], m.s[4], m.s[7]);
	Vector3 mz(m.s[2], m.s[5], m.s[8]);
	float f[] = {
		x.dot(mx), x.dot(my), x.dot(mz),
		y.dot(mx), y.dot(my), y.dot(mz),
		z.dot(mx), z.dot(my), z.dot(mz)
	};
	return Matrix3(f);
}


ostream &operator<<(ostream &stream, const Matrix3 &m)
{
	stream
	  << m.s[0] << ' ' << m.s[1] << ' ' << m.s[2] << endl
	  << m.s[3] << ' ' << m.s[4] << ' ' << m.s[5] << endl
	  << m.s[6] << ' ' << m.s[7] << ' ' << m.s[8] << endl;

	return stream;
}

/*****************************************************************************
 * Matrix4 implementation
 *****************************************************************************/
Matrix4::Matrix4()
{
	s[0] = 1.0f; s[1] = 0.0f; s[2] = 0.0f; s[3] = 0.0f;
	s[4] = 0.0f; s[5] = 1.0f; s[6] = 0.0f; s[7] = 0.0f;
	s[8] = 0.0f; s[9] = 0.0f; s[10] = 1.0f; s[11] = 0.0f;
	s[12] = 0.0f; s[13] = 1.0f; s[14] = 0.0f; s[15] = 1.0f;
}

Matrix4::Matrix4(const float *f)
{
	float *p = s;
	for (unsigned int i = 0; i < 16; i++)
		*p++ = *f++;

}

ostream &operator<<(ostream &stream, const Matrix4 &m)
{
	stream
	  << m.s[0] << ' ' << m.s[1] << ' ' << m.s[2] << ' ' << m.s[3] << endl
	  << m.s[4] << ' ' << m.s[5] << ' ' << m.s[6] << ' ' << m.s[7] << endl
	  << m.s[8] << ' ' << m.s[9] << ' ' << m.s[10] << ' ' << m.s[11] << endl
	  << m.s[12] << ' ' << m.s[13] << ' ' << m.s[14] << ' ' << m.s[15] << endl;

	return stream;
}
