/*****************************************************************************
 * Filename			NBody.cpp
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

#include "UnitTest.h"
#include "Weapon.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>
#include <iostream>

#include "Vector.h"

using namespace std;

#define M_RAD (180.0 / M_PI)
#define M_1_RAD (M_PI / 180.0)


void DebugVector3::Print(const char *const fmt)
{
	char buffer[200];
	sprintf(buffer, "%s %s %s\n", fmt, fmt, fmt);
	printf(buffer, s[0], s[1], s[2]);
}



#define STR_FORMAT3(F) \
	"%s %s %s\n%s %s %s\n%s %s %s\n%s %s %s\n", \
	F, F, F, F,	F, F, F, F,	F

void DebugMatrix3::Print(const char *const fmt)
{
	char buffer[200];
	sprintf(buffer, STR_FORMAT3(fmt));
	printf(buffer, s[0], s[1], s[2],
		s[3], s[4], s[5],
		s[6], s[7], s[8]);

}
	
void DebugMatrix3::GLPrint(const float *f, const char *const fmt)
{
	char buffer[200];
	sprintf(buffer, STR_FORMAT3(fmt));
	printf(buffer, f[0], f[4], f[8], f[12],
		f[1], f[5], f[9], f[13],
		f[2], f[6], f[10], f[14],
		f[3], f[7], f[11], f[15]);
}

#define STR_FORMAT4(F) \
	"%s %s %s %s\n%s %s %s %s\n%s %s %s %s\n%s %s %s %s\n", \
	F, F, F, F,	F, F, F, F,	F, F, F, F,	F, F, F, F


Vector4 DebugMatrix4::GLMult(const float *f, const Vector4 &v)
{
	return Vector4(
		Vector4(f[0], f[4], f[8], f[12]).dot(v),
		Vector4(f[1], f[5], f[9], f[13]).dot(v),
		Vector4(f[2], f[6], f[10], f[14]).dot(v),
		Vector4(f[3], f[7], f[11], f[15]).dot(v));
}

void DebugMatrix4::GLPrint(const float *f, const char *const fmt)
{
	char buffer[200];
	sprintf(buffer, STR_FORMAT4(fmt));
	printf(buffer, f[0], f[4], f[8], f[12],
		f[1], f[5], f[9], f[13],
		f[2], f[6], f[10], f[14],
		f[3], f[7], f[11], f[15]);
}





void UnitTestRun()
{
	Vector3 x(1.0f, 0.0f, 0.0f);
	Vector3 y(0.0f, 1.0f, 0.0f);
	Vector3 z(0.0f, 0.0f, 1.0f);

	Vector3 xx = -x;
	
	Matrix3 rotX = Matrix3::RotationX(M_PI_2);
	Matrix3 rotY = Matrix3::RotationY(M_PI_2);
	Matrix3 rotZ = Matrix3::RotationZ(M_PI_2);

	Matrix3 m;
	m[0][0] = 0; m[0][1] = 1; m[0][2] = 2;
	m[1][0] = 3; m[1][1] = 4; m[1][2] = 5;
	m[2][0] = 6; m[2][1] = 7; m[2][2] = 8;

	cout << m;

	const Matrix3 &n = m;


	cout.precision(1);

	cout << rotX * y;
	cout << rotY * z;
	cout << rotZ * x;

	x = rotY * x;
	y = rotZ * y;
	z = rotX * z;
	cout << "x:" << x << "y:" << y << "z:" << z;
	//cout.precision(5);

  double f = 3.14159;
  cout.setf(0,ios::floatfield);            // floatfield not set
  cout.precision(5);
  cout << f << endl;
  cout.precision(10);
  cout << f << endl;
  cout.setf(ios::fixed,ios::floatfield);   // floatfield set to fixed
  cout << f << endl;

	Grenade g(Point3(0.0f, 0.0f, 0.0f), M_PI_2, 0.5 * M_PI_2, 1.0f);
	return;
}

 