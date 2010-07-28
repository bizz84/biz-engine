/*****************************************************************************
 * Filename			Mesh.cpp
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

#include "Mesh.h"
#include <lib3ds/vector.h>
#include "assert.h"
#include "Timer.h"

const int permutation[3] = { 1, 2, 0 };

Mesh::Mesh(Lib3dsMesh *mesh, float scale/* = 1.0f*/,
		   bool flipNormals/* = false*/, int *permutation/* = NULL*/)
	: pVBO(NULL), pMesh(mesh), fScale(scale), bFlipNormals(flipNormals)
{
	assert(mesh != NULL);
	if (permutation == NULL)
	{
		iPermutation[0] = 1;
		iPermutation[1] = 2;
		iPermutation[2] = 0;
	}
	else
	{
		iPermutation[0] = permutation[0];
		iPermutation[1] = permutation[1];
		iPermutation[2] = permutation[2];
	}

	unsigned int i, j, p;

	Lib3dsVector *normalL = new Lib3dsVector[3 * pMesh->faces];
	lib3ds_mesh_calculate_normals(mesh, normalL);

	int offset = pMesh->texels == pMesh->points ? 8 : 6;

	// Bind all attributes in the same buffer
	float *attribs = new float [offset * pMesh->points];
	int *indices = new int[pMesh->faces * 3];
	for (i = 0; i < pMesh->faces; i++)
	{
		indices[3 * i + 0] = pMesh->faceL[i].points[0];
		indices[3 * i + 1] = pMesh->faceL[i].points[1];
		indices[3 * i + 2] = pMesh->faceL[i].points[2];
	}
	for (i = 0; i < pMesh->faces; i++)
	{
		for (j = 0; j < 3; j++)
		{
			p = pMesh->faceL[i].points[j];
			attribs[offset * p + 0] =
				scale * pMesh->pointL[p].pos[iPermutation[0]];
			attribs[offset * p + 1] =
				scale * pMesh->pointL[p].pos[iPermutation[1]];
			attribs[offset * p + 2] =
				scale * pMesh->pointL[p].pos[iPermutation[2]];

			// one normal per face
			//attribs[offset * p + 3] = pMesh->faceL[i].normal[iPermutation[0]];
			//attribs[offset * p + 4] = pMesh->faceL[i].normal[iPermutation[1]];
			//attribs[offset * p + 5] = pMesh->faceL[i].normal[iPermutation[2]];
			
			// one normal per vertex
			attribs[offset * p + 3] =
				NormalsSign() * normalL[3 * i + j][iPermutation[0]];
			attribs[offset * p + 4] =
				NormalsSign() * normalL[3 * i + j][iPermutation[1]];
			attribs[offset * p + 5] =
				NormalsSign() * normalL[3 * i + j][iPermutation[2]];

			if (pMesh->texelL)
			{
				attribs[offset * p + 6] = pMesh->texelL[p][0];
				attribs[offset * p + 7] = pMesh->texelL[p][1];
			}
			//printf("u=%.2f,v=%.2f\n",
			//	pMesh->texelL[p][0], pMesh->texelL[p][1]);
		}
	}


	pVBO = auto_ptr<IndexedVBO>(new IndexedVBO(attribs, sizeof(float) * offset,
		pMesh->points, indices, pMesh->faces * 3));
	pVBO->SetVertexData(0, 3);
	pVBO->SetNormalData(sizeof(float) * 3);
	if (pMesh->texelL)
		pVBO->SetTexCoordData(sizeof(float) * 6, 2);
	//pVBO->AddEntry(glVertexPointer, 3, GL_FLOAT, 0);

	delete [] indices;
	delete [] attribs;
	delete [] normalL;
}




