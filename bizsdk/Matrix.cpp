/*****************************************************************************
 * Filename			Matrix.cpp
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

#include "Matrix.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>

static const float z = 0.0f;
static const float u = 1.0f;
/*****************************************************************************
 * Matrix3 implementation
 *****************************************************************************/

Matrix3::Matrix3()
{
	s[0] = u; s[1] = z; s[2] = z;
	s[3] = z; s[4] = u; s[5] = z;
	s[6] = z; s[7] = z; s[8] = u;
}
// TODO: memcpy?
Matrix3::Matrix3(const float *f)
{
	float *p = s;
	for (unsigned int i = 0; i < 9; i++)
		*p++ = *f++;
}
Matrix3::Matrix3(const float f00, const float f01, const float f02, 
        const float f10, const float f11, const float f12, 
        const float f20, const float f21, const float f22)
{
	s[0] = f00; s[1] = f01; s[2] = f02;
	s[3] = f10; s[4] = f11; s[5] = f12;
	s[6] = f20; s[7] = f21; s[8] = f22;
}

const Matrix3 Matrix3::RotationX(const float angle)
{
	float c = cos(angle);
	float s = sin(angle);
	return Matrix3(
		u, z,  z,
		z, c, -s,
		z, s,  c);
}

const Matrix3 Matrix3::RotationY(const float angle)
{
	float c = cos(angle);
	float s = sin(angle);
	return Matrix3(
		 c, z, s,
		 z, u, z,
		-s, z, c);
}

const Matrix3 Matrix3::RotationZ(const float angle)
{
	float c = cos(angle);
	float s = sin(angle);
	return Matrix3(
		c, -s, z,
		s,  c, z,
		z,  z, u);
}

const Vector3 Matrix3::operator*(const Vector3 &v) const
{
	return Vector3(v.dot(s+0), v.dot(s+3), v.dot(s+6));
}

const Matrix3 Matrix3::operator*(const Matrix3 &m) const
{
	Vector3 mx(m.s[0], m.s[3], m.s[6]);
	Vector3 my(m.s[1], m.s[4], m.s[7]);
	Vector3 mz(m.s[2], m.s[5], m.s[8]);
	float f[] = {
		mx.dot(s+0), my.dot(s+0), mz.dot(s+0),
		mx.dot(s+3), my.dot(s+3), mz.dot(s+3),
		mx.dot(s+6), my.dot(s+6), mz.dot(s+6),
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
	s[0]  = u; s[1]  = z; s[2]  = z; s[3]  = z;
	s[4]  = z; s[5]  = u; s[6]  = z; s[7]  = z;
	s[8]  = z; s[9]  = z; s[10] = u; s[11] = z;
	s[12] = z; s[13] = z; s[14] = z; s[15] = u;
}

Matrix4::Matrix4(
		const float f00, const float f01, const float f02, const float f03,
	    const float f10, const float f11, const float f12, const float f13,  
	    const float f20, const float f21, const float f22, const float f23,  
	    const float f30, const float f31, const float f32, const float f33)
{
	s[0]  = f00; s[1]  = f01; s[2]  = f02; s[3]  = f03;
	s[4]  = f10; s[5]  = f11; s[6]  = f12; s[7]  = f13;
	s[8]  = f20; s[9]  = f21; s[10] = f22; s[11] = f23;
	s[12] = f30; s[13] = f31; s[14] = f32; s[15] = f33;
}

Matrix4::Matrix4(const float *f)
{
	float *p = s;
	for (unsigned int i = 0; i < 16; i++)
		*p++ = *f++;
}

Matrix4::Matrix4(const Matrix3 &mat3)
{
	s[0] = mat3[0][0]; s[1] = mat3[0][1]; s[ 2] = mat3[0][2]; s[ 3] = z;
	s[4] = mat3[1][0]; s[5] = mat3[1][1]; s[ 6] = mat3[1][2]; s[ 7] = z;
	s[8] = mat3[2][0]; s[9] = mat3[2][1]; s[10] = mat3[2][2]; s[11] = z;
	s[12] = z;         s[13] = z;         s[14] = z;          s[15] = u;
}


Matrix4 Matrix4::Zero()
{
	Matrix4 m;
	memset(m.s, '\0', sizeof(float) * 16);
	return m;
}

const Matrix4 Matrix4::operator-() const
{
	return Matrix4(
		-s[0], -s[1], -s[2], -s[3],
		-s[4], -s[5], -s[6], -s[7],
		-s[8], -s[9], -s[10], -s[11],
		-s[12], -s[13], -s[14], -s[15]);
}


Matrix4 Matrix4::operator*(const Matrix4 &m) const
{
	Vector4 mx(m.s[0], m.s[4], m.s[8], m.s[12]);
	Vector4 my(m.s[1], m.s[5], m.s[9], m.s[13]);
	Vector4 mz(m.s[3], m.s[6], m.s[10], m.s[14]);
	Vector4 mw(m.s[4], m.s[7], m.s[11], m.s[15]);
	float f[] = {
		mx.dot(s+0), my.dot(s+0), mz.dot(s+0), mw.dot(s+0),
		mx.dot(s+4), my.dot(s+4), mz.dot(s+4), mw.dot(s+4),
		mx.dot(s+8), my.dot(s+8), mz.dot(s+8), mw.dot(s+8),
		mx.dot(s+12), my.dot(s+12), mz.dot(s+12), mw.dot(s+12)
	};
	return Matrix4(f);
}


const Matrix4 Matrix4::Transpose() const
{
	Matrix4 m;
	m[0][0] = s[0]; m[0][1] = s[4]; m[0][2] = s[8]; m[0][3] = s[12];
	m[1][0] = s[1]; m[1][1] = s[5]; m[1][2] = s[9]; m[1][3] = s[13];
	m[2][0] = s[2]; m[2][1] = s[6]; m[2][2] = s[10]; m[2][3] = s[14];
	m[3][0] = s[3]; m[3][1] = s[7]; m[3][2] = s[11]; m[3][3] = s[15];
	return m;
}


const Matrix4 Matrix4::Inverse(const float epsilon) const
{
	float a0 = s[ 0]*s[ 5] - s[ 1]*s[ 4];
    float a1 = s[ 0]*s[ 6] - s[ 2]*s[ 4];
    float a2 = s[ 0]*s[ 7] - s[ 3]*s[ 4];
    float a3 = s[ 1]*s[ 6] - s[ 2]*s[ 5];
    float a4 = s[ 1]*s[ 7] - s[ 3]*s[ 5];
    float a5 = s[ 2]*s[ 7] - s[ 3]*s[ 6];
    float b0 = s[ 8]*s[13] - s[ 9]*s[12];
    float b1 = s[ 8]*s[14] - s[10]*s[12];
    float b2 = s[ 8]*s[15] - s[11]*s[12];
    float b3 = s[ 9]*s[14] - s[10]*s[13];
    float b4 = s[ 9]*s[15] - s[11]*s[13];
    float b5 = s[10]*s[15] - s[11]*s[14];

    float det = a0*b5 - a1*b4 + a2*b3 + a3*b2 - a4*b1 + a5*b0;
    if (fabs(det) > epsilon)
    {
        Matrix4 inverse;
        inverse.s[ 0] = + s[ 5]*b5 - s[ 6]*b4 + s[ 7]*b3;
        inverse.s[ 4] = - s[ 4]*b5 + s[ 6]*b2 - s[ 7]*b1;
        inverse.s[ 8] = + s[ 4]*b4 - s[ 5]*b2 + s[ 7]*b0;
        inverse.s[12] = - s[ 4]*b3 + s[ 5]*b1 - s[ 6]*b0;
        inverse.s[ 1] = - s[ 1]*b5 + s[ 2]*b4 - s[ 3]*b3;
        inverse.s[ 5] = + s[ 0]*b5 - s[ 2]*b2 + s[ 3]*b1;
        inverse.s[ 9] = - s[ 0]*b4 + s[ 1]*b2 - s[ 3]*b0;
        inverse.s[13] = + s[ 0]*b3 - s[ 1]*b1 + s[ 2]*b0;
        inverse.s[ 2] = + s[13]*a5 - s[14]*a4 + s[15]*a3;
        inverse.s[ 6] = - s[12]*a5 + s[14]*a2 - s[15]*a1;
        inverse.s[10] = + s[12]*a4 - s[13]*a2 + s[15]*a0;
        inverse.s[14] = - s[12]*a3 + s[13]*a1 - s[14]*a0;
        inverse.s[ 3] = - s[ 9]*a5 + s[10]*a4 - s[11]*a3;
        inverse.s[ 7] = + s[ 8]*a5 - s[10]*a2 + s[11]*a1;
        inverse.s[11] = - s[ 8]*a4 + s[ 9]*a2 - s[11]*a0;
        inverse.s[15] = + s[ 8]*a3 - s[ 9]*a1 + s[10]*a0;

        float invDet = ((float)1)/det;
        inverse.s[ 0] *= invDet;
        inverse.s[ 1] *= invDet;
        inverse.s[ 2] *= invDet;
        inverse.s[ 3] *= invDet;
        inverse.s[ 4] *= invDet;
        inverse.s[ 5] *= invDet;
        inverse.s[ 6] *= invDet;
        inverse.s[ 7] *= invDet;
        inverse.s[ 8] *= invDet;
        inverse.s[ 9] *= invDet;
        inverse.s[10] *= invDet;
        inverse.s[11] *= invDet;
        inverse.s[12] *= invDet;
        inverse.s[13] *= invDet;
        inverse.s[14] *= invDet;
        inverse.s[15] *= invDet;

        return inverse;
    }

    return Zero();
}

Matrix4 Matrix4::Translation(const Vector3 &t)
{
	return Matrix4(u, z, z, t[0],
	               z, u, z, t[1],
				   z, z, u, t[2],
				   z, z, z, u);
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
