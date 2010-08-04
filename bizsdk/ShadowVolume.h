/*****************************************************************************
 * Filename			ShadowVolume.h
 * 
 * License			GPLv3
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

#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>

#include <lib3ds/mesh.h>

#include "VBO.h"
#include "Mesh.h"

#include <vector>
using namespace std;

class ShadowVolumeMesh : public Mesh
{
protected:
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
		bool operator==(const Vector3 &rhs)
		{
			return v[0] == rhs.v[0] && v[1] == rhs.v[1] && v[2] == rhs.v[2];
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
		Vector3 v1;
		Vector3 v2;
		Vector3 normal;

		Edge(Vector3 p1, Vector3 p2, Vector3 n) : v1(p1), v2(p2), normal(n) { }

		bool operator==(const Edge &e)
		{
			return (v1 == e.v1 && v2 == e.v2) || (v1 == e.v2 && v2 == e.v1);
		}
	};

	bool ProcessEdge(vector<Edge> &pairs,
		const Edge &e, vector<VectorAndNormal> &buffer);

	static void Dump(vector<VectorAndNormal> &buffer, FILE *fp);
	static bool Read(unsigned int &elements, float *&data, FILE *fp);

	auto_ptr<VBO> pShadowVolumeVBO;

public:
	ShadowVolumeMesh(Lib3dsMesh *mesh, float scale = 1.0f,
	                 bool flipNormals = false, int *permutation = NULL);

	const VBO *GetShadowVolumeVBO() const { return pShadowVolumeVBO.get(); }
};

#endif
