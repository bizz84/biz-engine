/*****************************************************************************
 * Filename			ShadowVolume.h
 * 
 * License			LGPL
 *
 * Author			Andrea Bizzotto (bizz84@gmail.com)
 *
 * Platform			LinuxX11 / OpenGL
 * 
 * Description		Lighting & Shadows demo using stencil buffers
 *
 *****************************************************************************/

#ifndef _SHADOW_VOLUME_H_
#define _SHADOW_VOLUME_H_

#include <math.h>
#include <stdio.h>

#include <lib3ds/mesh.h>

#include "VBO.h"

class ShadowVolume
{
	struct Vector3
	{
		float v[3];
		Vector3() { }
		Vector3(float *p)
		{
			v[0] = p[0];
			v[1] = p[1];
			v[2] = p[2];
		}
		Vector3(float v0, float v1, float v2)
		{
			v[0] = v0;
			v[1] = v1;
			v[2] = v2;
		}

	};
	struct VectorAndNormal
	{
		Vector3 v;
		Vector3 n;

		VectorAndNormal() { }
		VectorAndNormal(float *ptr) : v(ptr[0], ptr[1], ptr[2]),
			n(ptr[3], ptr[4], ptr[5]) { }
		VectorAndNormal(const Vector3 &v, const Vector3 &n) : v(v), n(n) { }
	};

	struct Edge
	{
		Lib3dsWord v1;
		Lib3dsWord v2;
		unsigned int face;

		Edge(Lib3dsWord p1, Lib3dsWord p2, unsigned int f) : v1(p1), v2(p2), face(f) { }

		bool operator==(const Edge &e)
		{
			return (v1 == e.v1 && v2 == e.v2) || (v1 == e.v2 && v2 == e.v1);
		}
	};

	static bool ProcessEdge(Lib3dsMesh *mesh, vector<Edge> &pairs, const Edge &e, vector<VectorAndNormal> &buffer);

	static void Dump(vector<VectorAndNormal> &buffer, FILE *fp);
	static bool Read(unsigned int &elements, float *&data, FILE *fp);
	VBO *pVBO;
public:
	ShadowVolume(Lib3dsMesh *mesh);
	~ShadowVolume();

	VBO *GetVBO() { return pVBO; }
};

#endif
