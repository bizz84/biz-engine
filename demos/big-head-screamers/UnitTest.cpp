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
#include "Bullet.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>
#include <iostream>

#include "Vector.h"
#include "GLResourceManager.h"

#include <list>

#include "boost/shared_ptr.hpp"
#include "boost/ptr_container/ptr_list.hpp"
#include "boost/ptr_container/ptr_vector.hpp"

using namespace boost;


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


	//matrix = [0.55 0 0 0, 0 0.4 -0.08 0, 0 12.3 -34.32 -0.05, 0 12.19 33.35 0.05]
	//matrxx = [0.55 0 0 0, 0 0.4 -12.3 12.2, 0 -0.08 -34.32 33.33 0 0 -0.05 0.05]
	//matrxx = [0.55 0 0 0, 0 0.4 -0.08 0, 0 -12.3 -34.32 -0.05, 0 12.20 33.33 0.05]
	float pvrp[] = {
		1.81, 0, 0, 0,
		0, 2.414, 0, 0,
		0, 0, -1, -20,
		0, 0, -1, 0
	};
	float pvrm[] = {
		1, 0, 0, 0,
		0, 0.98, -0.195, -109,
		0, 0.195, 0.98, -721,
		0, 0, 0, 1,
	};
	Matrix4 inv = (Matrix4(pvrp) * Matrix4(pvrm)).Inverse(0.1);

	/*  double f = 3.14159;
  cout.setf(0,ios::floatfield);            // floatfield not set
  cout.precision(5);
  cout << f << endl;
  cout.precision(10);
  cout << f << endl;
  cout.setf(ios::fixed,ios::floatfield);   // floatfield set to fixed
  cout << f << endl;*/


  	/*float proj[16], mview[16];

	Vector4 v[4];

	float t = 100.0f;
	float w = 0.0001f;
	v[0] = Vector4(1.0f, 0.0f, 1.0f, w);
	v[1] = Vector4(-1.0f, 0.0f, 1.0f, w);
	v[2] = Vector4(-1.0f, 0.0f, -1.0f, w);
	v[3] = Vector4(1.0f, 0.0f, -1.0f, w);


  	glGetFloatv(GL_PROJECTION_MATRIX, proj);
	glGetFloatv(GL_MODELVIEW_MATRIX, mview);
	Matrix4 projView = Matrix4(proj) * Matrix4(mview);

	Vector4 u[4];
	Vector4 x[4];
	for (unsigned int i = 0; i < 4; i++)
	{
		Projection(u[i], proj, mview, v[i]);
		ZeroZ(x[i], u[i]);
	}*/

	/*if (KeyPressed(KEY_1))
	{
		cout << "Modelview matrix\n";
		DebugMatrix4::GLPrint(mview, "%5.2f");
		cout << "Projection matrix\n";
		DebugMatrix4::GLPrint(proj, "%5.2f");

		for (unsigned int i = 0; i < 4; i++)
		{
			printf("Projection of [%.1f, %.1f, %.1f, %.1f] is "\
				"[%.1f, %.1f, %.1f, %.1f]\n",
				v[i][0], v[i][1], v[i][2], v[i][3],
				u[i][0], u[i][1], u[i][2], u[i][3]);
		}
	}*/


	return;
}

 

bool AttribTest()
{
	GLResourceManager &loader = GLResourceManager::Instance();
	GLuint texture, program;
	if (!loader.LoadTextureFromFile("data/textures/Sprites/BER01.bmp",
			texture, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR))
		return false;

	glBindTexture(GL_TEXTURE_2D, texture);

	if (!loader.LoadShaderFromFile("data/shaders/TestAttrib.vert", "data/shaders/Lookup.frag", program))
		return false;


	glUseProgram(program);

	//glEnableClientState(GL_VERTEX_ARRAY);	
	//glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	float x = -0.5, y = -0.5, w = 1.0, h = 1.0, u0 = 0.0, v0 = 0.0, u1 = 1.0, v1 = 1.0;

	float afAttribs[] = {
		x    , y    , u0, v1,
		x + w, y    , u1, v1,
		x + w, y + h, u1, v0,
		x    , y + h, u0, v0
	};

	glVertexPointer(2, GL_FLOAT, sizeof(float) * 4, afAttribs);
	glTexCoordPointer(2, GL_FLOAT, sizeof(float) * 4, afAttribs + 2);

	GLint attribLoc[2];

	attribLoc[0] = glGetAttribLocation(program, "inVertex");
	attribLoc[1] = glGetAttribLocation(program, "inTexCoord");


	// Pass in attributes
	// This one shouldn't be necessary if glVertexPointer is issued
	glEnableVertexAttribArray(attribLoc[0]);
	glVertexAttribPointer(attribLoc[0], 2, GL_FLOAT, GL_FALSE,
		sizeof(float) * 4, afAttribs);
	glEnableVertexAttribArray(attribLoc[1]);
	glVertexAttribPointer(attribLoc[1], 2, GL_FLOAT, GL_FALSE,
		sizeof(float) * 4, afAttribs + 2);

	glDrawArrays(GL_QUADS, 0, 4);

	glDisableVertexAttribArray(attribLoc[0]);
	glDisableVertexAttribArray(attribLoc[1]);


	//glDisableClientState(GL_VERTEX_ARRAY);	
	//glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	return true;
}

class A
{
public:
	A(int a) : a(a) { }
	virtual ~A() { }
	int a;
	virtual void f() { cout << "A(" << a << ")\n"; }
};

class B : public A
{
public:
	B(int a) : A(a) { }
	virtual void f() { cout << "B(" << a << ")\n"; }
};

class C : public A
{
public:
	C(int a) : A(a) { }
	virtual void f() { cout << "C(" << a << ")\n"; }
};

void TestList()
{
	list<A*> a;
	a.push_back(new A(1));
	a.push_back(new A(2));
	a.push_back(new A(3));
	
	list<A*> b;
	b.push_back(new B(4));
	b.push_back(new B(5));
	b.push_back(new B(6));

	list<A*> c;
	c.push_back(new C(7));
	c.push_back(new C(8));
	c.push_back(new C(9));

	{
		cout << "size(A) = " << a.size() << "size(B) = " << b.size() << "size(C) = " << c.size() << endl; 

		list<A*> d;
		d.merge(a);
		d.merge(b);
		d.merge(c);

		cout << "size(A) = " << a.size() << "size(B) = " << b.size() << "size(C) = " << c.size() << "size(D) = " << d.size() << endl; 

		list<A*>::iterator i;
		for (i = d.begin(); i != d.end(); i++)
			(*i)->f();
	}
	a.clear();
	{
		list<A*> d;
		d.merge(a);
		d.merge(b);
		d.merge(c);

		cout << "size(A) = " << a.size() << "size(B) = " << b.size() << "size(C) = " << c.size() << "size(D) = " << d.size(); 

		list<A*>::iterator i;
		for (i = d.begin(); i != d.end(); i++)
			(*i)->f();
	}

	ptr_vector<A> d;
	d.push_back(new B(0));
	ptr_vector<A>::const_iterator i = d.begin();

	dynamic_cast<const B*>(&*i);
	
	shared_ptr<A*> p;

	return;
}
