/*****************************************************************************
 * Filename			Matrix.h
 * 
 * License			GPLv3
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
#include <math.h>

// TODO: Use template metaprogramming to define these classes. 

// Note that function signatures are "similar" across these classes.
// Conceptually all Vector classes could derive from a base class defining
// interfaces via virtual functions.
// This is not recommended here since this would generate a virtual table that
// would increase the size of the instances, in addition to slowing down the
// performance

//! Class defining a vector of two elements and common inherent operations
class Vector2
{
protected:
	float s[2];
public:
	Vector2();
	Vector2(const float *f);
	Vector2(const float x, const float y);

	float &operator[] (int i) { return s[i]; }
	const float &operator[] (int i) const { return s[i]; }

	const float Length() const { return sqrt(this->dot(*this)); }
	const Vector2 Normalize() const { return *this / Length(); }

	inline const float dot(const Vector2 &v) const
	{
		return s[0] * v[0] + s[1] * v[1];
	}
	inline const Vector2 operator *(const float scalar) const
	{
		return Vector2(s[0] * scalar, s[1] * scalar);
	}
	inline const Vector2 operator /(const float scalar) const
	{
		return Vector2(s[0] / scalar, s[1] / scalar);
	}

	inline const Vector2 operator +(const Vector2 &rhs) const
	{
		return Vector2(s[0] + rhs[0], s[1] + rhs[1]);
	}

	inline const Vector2 operator -(const Vector2 &rhs) const
	{
		return Vector2(s[0] - rhs[0], s[1] - rhs[1]);
	}

	inline Vector2 &operator +=(const Vector2 &v)
	{
		s[0] += v[0];
		s[1] += v[1];
		return *this;
	}	
	inline Vector2 &operator -=(const Vector2 &v)
	{
		s[0] -= v[0];
		s[1] -= v[1];
		return *this;
	}	
};
//! Synonym for Vector2 (conceptually different)
typedef Vector2 Point2;

//! Class defining a vector of three elements and common inherent operations
class Vector3
{
protected:
	float s[3];
public:
	Vector3();
	Vector3(const float *f);
	Vector3(const float x, const float y, const float z);

	float &operator[] (int i) { return s[i]; }
	const float &operator[] (int i) const { return s[i]; }

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
	inline const Vector3 operator /(const float scalar) const
	{
		return Vector3(s[0] / scalar, s[1] / scalar, s[2] / scalar);
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
	inline const float dot(const float *f) const
	{
		return s[0] * f[0] + s[1] * f[1] + s[2] * f[2];
	}
	inline const float operator *(const float *f) const
	{
		return this->dot(f);
	}

	inline const bool operator==(const Vector3 &v) const
	{
		return s[0] == v[0] && s[1] == v[1] && s[2] == v[2];
	}
	inline const bool operator==(const float *f) const
	{
		return s[0] == f[0] && s[1] == f[1] && s[2] == f[2];
	}
	inline const bool operator!=(const Vector3 &v) const
	{
		return !(*this == v);
	}
	inline const bool operator!=(const float *f) const
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

	const float Length() const { return sqrt(this->dot(*this)); }
	const Vector3 Normalize() const { return *this / Length(); }
};

//! Synonym for Vector3 (conceptually different)
typedef Vector3 Point3;

//! Class defining a vector of four elements and common inherent operations
// TODO: Add all functions as in Vector3
class Vector4
{
protected:
	float s[4];
public:
	Vector4();
	Vector4(const float *f);
	Vector4(const float x, const float y, const float z, const float w);

	float &operator[] (int i) { return s[i]; }
	const float &operator[] (int i) const { return s[i]; }

	inline const Vector4 operator *(const float scalar) const
	{
		return Vector4(s[0] * scalar, s[1] * scalar,
		               s[2] * scalar, s[3] * scalar);
	}
	inline const Vector4 operator /(const float scalar) const
	{
		return Vector4(s[0] / scalar, s[1] / scalar,
		               s[2] / scalar, s[3] / scalar);
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
	inline const float dot(const float *f) const
	{
		return s[0]*f[0] + s[1]*f[1] + s[2]*f[2] + s[3]*f[3];
	}
	inline const float operator*(const float *f) const
	{
		return this->dot(f);
	}

	//
	inline const bool operator==(const Vector4 &v) const
	{
		return s[0] == v[0] && s[1] == v[1] && s[2] == v[2] && s[3] == v[3];
	}
	inline const bool operator==(const float *f) const
	{
		return s[0] == f[0] && s[1] == f[1] && s[2] == f[2] && s[3] == f[3];
	}
	inline const bool operator!=(const float *f) const
	{
		return !(*this == f);
	}
	inline const bool operator!=(const Vector4 &v) const
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

	const float Length() const { return sqrt(this->dot(*this)); }
	const Vector4 Normalize() const { return *this / Length(); }

	//void AssignTo(float *f);
};

//! Synonym for Vector4 (conceptually different)
typedef Vector4 Point4;

#endif


