/*****************************************************************************
 * Filename			Mesh.h
 * 
 * License			LGPL
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


#include <SDL/SDL.h>
#include "Extensions.h"
#include "VBO.h"
#include <lib3ds/mesh.h>

class Mesh
{
protected:
	float fScale;
	bool bFlipNormals;
	int iPermutation[3];
	Lib3dsMesh *pMesh;

	IndexedVBO *pVBO;
	VBO *pShadowVolumeVBO;

	float NormalsSign() { return bFlipNormals ? -1.0f : 1.0f; }
public:
	Mesh(Lib3dsMesh *mesh, float scale = 1.0f, bool flipNormals = false, int *permutation = NULL);
	~Mesh();

	IndexedVBO *GetVBO() { return pVBO; }
};

#endif
