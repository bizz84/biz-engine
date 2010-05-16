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
 * Vector2 implementation
 *****************************************************************************/
Vector2::Vector2()
{
	s[0] = s[1] = 0.0f;
}

Vector2::Vector2(const float *f)
{
	s[0] = f[0];
	s[1] = f[1];
}
Vector2::Vector2(const float x, const float y)
{
	s[0] = x;
	s[1] = y;
}

/*****************************************************************************
 * Vector3 implementation
 *****************************************************************************/
Vector3::Vector3()
{
	s[0] = s[1] = s[2] = 0.0f;
}

Vector3::Vector3(const float *f)
{
	s[0] = f[0];
	s[1] = f[1];
	s[2] = f[2];
}

Vector3::Vector3(const float x, const float y, const float z)
{
	s[0] = x;
	s[1] = y;
	s[2] = z;
}

ostream &operator<<(ostream &stream, const Vector3 &v)
{
	stream
	  << v.s[0] << ' ' << v.s[1] << ' ' << v.s[2] << endl;
	return stream;
}


/*****************************************************************************
 * Vector4 implementation
 *****************************************************************************/

Vector4::Vector4()
{
	s[0] = s[1] = s[2] = s[3] = 0.0f;
}

Vector4::Vector4(const float *f)
{
	float *p = s;
	*p++ = *f++;
	*p++ = *f++;
	*p++ = *f++;
	*p++ = *f++;
}
Vector4::Vector4(const float x, const float y, const float z, const float w)
{
	float *p = s;
	*p++ = x;
	*p++ = y;
	*p++ = z;
	*p++ = w;

}

float Vector4::dot(const Vector4 &v)
{
	return s[0]*v.s[0] + s[1]*v.s[1] + s[2]*v.s[2] + s[3]*v.s[3];
}

float Vector4::dot(const float *f)
{
	return s[0]*f[0] + s[1]*f[1] + s[2]*f[2] + s[3]*f[3];
}

void Vector4::AssignTo(float *f)
{
	float *p = s;
	*f++ = *p++;
	*f++ = *p++;
	*f++ = *p++;
	*f++ = *p++;
}

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

Matrix3 Matrix3::RotationX(const float angle)
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

Matrix3 Matrix3::RotationY(const float angle)
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

Matrix3 Matrix3::RotationZ(const float angle)
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

Vector3 Matrix3::operator*(const Vector3 &v)
{
	return Vector3(
		Vector3(s+0).dot(v),
		Vector3(s+3).dot(v),
		Vector3(s+6).dot(v));
}

Matrix3 Matrix3::operator*(const Matrix3 &m)
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
