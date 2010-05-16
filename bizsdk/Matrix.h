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
#include <iostream>

using namespace std;

class Vector
{

public:
	virtual float &operator[] (int i) = 0;
};

/*****************************************************************************
 * Vector2 class definition
 *****************************************************************************/

class Vector2 : public Vector
{
protected:
	float s[2];
public:
	Vector2();
	Vector2(const float *f);
	Vector2(const float x, const float y);

	virtual float &operator[] (int i) { return s[i]; }
	virtual const float &operator[] (int i) const { return s[i]; }
};

/*****************************************************************************
 * Vector3 class definition
 *****************************************************************************/

class Vector3 : public Vector
{
protected:
	float s[3];
public:
	Vector3();
	Vector3(const float *f);
	Vector3(const float x, const float y, const float z);

	virtual float &operator[] (int i) { return s[i]; }
	virtual const float &operator[] (int i) const { return s[i]; }

	inline Vector3 &operator +=(const Vector3 &rhs)
	{
		s[0] += rhs.s[0];
		s[1] += rhs.s[1];
		s[2] += rhs.s[2];
		return *this;
	}
	inline const Vector3 operator *(const float &scalar) const
	{
		return Vector3(s[0] * scalar, s[1] * scalar, s[2] * scalar);
	}

	inline const float dot(const Vector3 &v) const
	{
		return s[0] * v.s[0] + s[1] * v.s[1] + s[2] * v.s[2];
	}

	inline const bool operator==(const Vector3 &v) const
	{
		return s[0] == v.s[0] && s[1] == v.s[1] && s[2] == v.s[2];
	}
	inline const bool operator!=(const Vector3 &v) const
	{
		return !(*this == v);
	}
	inline void Translate(const float x, const float y, const float z)
	{
		s[0] += x;
		s[1] += y;
		s[2] += z;
	}
	inline const Vector3 operator-() const 
	{
		return Vector3(-s[0], -s[1], -s[2]);
	}

	friend ostream &operator<<(ostream &stream, const Vector3 &v);

};

/*****************************************************************************
 * Vector4 class definition
 *****************************************************************************/

class Vector4 : public Vector
{
protected:
	float s[4];
public:
	Vector4();
	Vector4(const float *f);
	Vector4(const float x, const float y, const float z, const float w);

	float dot(const Vector4 &v);
	float dot(const float *f);

	virtual float &operator[] (int i) { return s[i]; }
	virtual const float &operator[] (int i) const { return s[i]; }

	void AssignTo(float *f);

	//friend ostream &operator<<(ostream &stream, const Vector4 &m);
};


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
	static Matrix3 RotationX(const float angle);
	static Matrix3 RotationY(const float angle);
	static Matrix3 RotationZ(const float angle);

	Vector3 operator*(const Vector3 &v);
	Matrix3 operator*(const Matrix3 &m);
	
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

	friend ostream &operator<<(ostream &stream, const Matrix4 &m);
};

typedef Vector3 Point3;
typedef Vector4 Point4;

#endif


