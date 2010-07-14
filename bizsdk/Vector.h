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
#ifndef _VECTOR_H_
#define _VECTOR_H_

#include "Extensions.h"
#include <iostream>

using namespace std;

class Vector
{

public:
	virtual float &operator[] (int i) = 0;
	virtual const float &operator[] (int i) const = 0;

	virtual const float dot(const float *f) const = 0;
	virtual const float operator*(const float *f) const = 0;

	virtual const bool operator==(const float *f) const = 0;
	virtual const bool operator!=(const float *f) const = 0;
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

	inline Vector3 &operator +=(const Vector3 &v)
	{
		s[0] += v[0];
		s[1] += v[1];
		s[2] += v[2];
		return *this;
	}
	inline const Vector3 operator *(const float scalar) const
	{
		return Vector3(s[0] * scalar, s[1] * scalar, s[2] * scalar);
	}
	//
	inline const float dot(const Vector3 &v) const
	{
		return s[0] * v[0] + s[1] * v[1] + s[2] * v[2];
	}
	inline const float operator *(const Vector3 &v) const
	{
		return this->dot(v);
	}
	//
	inline virtual const float dot(const float *f) const
	{
		return s[0] * f[0] + s[1] * f[1] + s[2] * f[2];
	}
	inline virtual const float operator *(const float *f) const
	{
		return this->dot(f);
	}

	inline const bool operator==(const Vector3 &v) const
	{
		return s[0] == v[0] && s[1] == v[1] && s[2] == v[2];
	}
	inline virtual const bool operator==(const float *f) const
	{
		return s[0] == f[0] && s[1] == f[1] && s[2] == f[2];
	}
	inline const bool operator!=(const Vector3 &v) const
	{
		return !(*this == v);
	}
	inline virtual const bool operator!=(const float *f) const
	{
		return !(*this != f);
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

	inline const Vector3 operator +(const Vector3 &rhs) const
	{
		return Vector3(s[0] + rhs[0], s[1] + rhs[1], s[2] + rhs[2]);
	}

	inline const Vector3 operator -(const Vector3 &rhs) const
	{
		return Vector3(s[0] - rhs[0], s[1] - rhs[1], s[2] - rhs[2]);
	}

	friend ostream &operator<<(ostream &stream, const Vector3 &v);

};

typedef Vector3 Point3;

/*****************************************************************************
 * Vector4 class definition
 *****************************************************************************/

// TODO: Add all functions as in Vector3
class Vector4 : public Vector
{
protected:
	float s[4];
public:
	Vector4();
	Vector4(const float *f);
	Vector4(const float x, const float y, const float z, const float w);

	virtual float &operator[] (int i) { return s[i]; }
	virtual const float &operator[] (int i) const { return s[i]; }

	inline const Vector4 operator *(const float scalar) const
	{
		return Vector4(s[0] * scalar, s[1] * scalar,
		               s[2] * scalar, s[3] * scalar);
	}
	//
	inline const float dot(const Vector4 &v) const
	{
		return s[0]*v[0] + s[1]*v[1] + s[2]*v[2] + s[3]*v[3];
	}
	inline const float operator*(const Vector4 &v) const
	{
		return this->dot(v);
	}
	//
	inline virtual const float dot(const float *f) const
	{
		return s[0]*f[0] + s[1]*f[1] + s[2]*f[2] + s[3]*f[3];
	}
	inline virtual const float operator*(const float *f) const
	{
		return this->dot(f);
	}

	//
	inline virtual const bool operator==(const Vector4 &v) const
	{
		return s[0] == v[0] && s[1] == v[1] && s[2] == v[2] && s[3] == v[3];
	}
	inline virtual const bool operator==(const float *f) const
	{
		return s[0] == f[0] && s[1] == f[1] && s[2] == f[2] && s[3] == f[3];
	}
	inline virtual const bool operator!=(const float *f) const
	{
		return !(*this == f);
	}
	inline virtual const bool operator!=(const Vector4 &v) const
	{
		return !(*this == v);
	}

	inline Vector4 &operator +=(const Vector4 &v)
	{
		s[0] += v[0];
		s[1] += v[1];
		s[2] += v[2];
		s[3] += v[3];
		return *this;
	}
	void AssignTo(float *f);

	//friend ostream &operator<<(ostream &stream, const Vector4 &m);
};

typedef Vector4 Point4;

#endif


