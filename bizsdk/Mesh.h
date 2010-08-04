/*****************************************************************************
 * Filename			Mesh.h
 * 
 * License			GPLv3
 *
 * Author			Andrea Bizzotto (bizz84@gmail.com)
 *
 * Platform			LinuxX11 / OpenGL
 * 
 * Description		Mesh utilities
 *
 *****************************************************************************/

#ifndef _MESH_H_
#define _MESH_H_


#ifdef __linux__
#include <SDL/SDL.h>
#else
#include <SDL.h>
#endif
#include "Extensions.h"
#include "VBO.h"
#include <lib3ds/mesh.h>

using namespace std;

// TODO: Find elegant solution for this
extern const int permutation[3];

class Mesh
{
protected:
	float fScale;
	bool bFlipNormals;
	int iPermutation[3];
	Lib3dsMesh *pMesh;

	auto_ptr<IndexedVBO> pVBO;

	const float NormalsSign() const { return bFlipNormals ? -1.0f : 1.0f; }
public:
	Mesh(Lib3dsMesh *mesh, float scale = 1.0f, bool flipNormals = false,
		int *permutation = NULL);

	const IndexedVBO *GetVBO() const { return pVBO.get(); }

	const int GetNumTriangles() const { return pMesh->faces; }
};

#endif
