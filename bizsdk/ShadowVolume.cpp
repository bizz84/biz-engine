/*****************************************************************************
 * Filename			ShadowVolume.cpp
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

#include "ShadowVolume.h"
#include "GLResourceManager.h"
#include <vector>
#include "Timer.h"
#include "assert.h"

using namespace std;

ShadowVolumeMesh::ShadowVolumeMesh(Lib3dsMesh *mesh, float scale/* = 1.0f*/,
	bool flipNormals/* = false*/, int *permutation/* = NULL*/)
	: Mesh(mesh, scale, flipNormals, permutation)
{
	char szShadowVolumeFile[50];
	sprintf(szShadowVolumeFile, "data/scenes/%s.sv", pMesh->name);
	FILE *fp = fopen(szShadowVolumeFile, "rb");
	if (fp == NULL)
	{
		Timer tsw;
		printf("Shadow volume for mesh %s does not exist. Creating it...\n",
			pMesh->name);
		printf("Statistics for mesh \"%s\": faces=%d,points=%d\n",
			pMesh->name, pMesh->faces, pMesh->points);

		vector<Edge> pairs;
		vector<VectorAndNormal> buffer(pMesh->faces * 3);

		// First, create buffer containing initial geometry
		// This will be a normal GL_TRIANGLES vbo
		unsigned int i;
		for (i = 0; i < pMesh->faces; i++)
		{
			Lib3dsFace &f = pMesh->faceL[i];
			Vector3 normal(
				NormalsSign() * f.normal[iPermutation[0]],
				NormalsSign() * f.normal[iPermutation[1]],
				NormalsSign() * f.normal[iPermutation[2]]);

			Vector3 point[3] = {
				Vector3(
					fScale * pMesh->pointL[f.points[0]].pos[iPermutation[0]],
					fScale * pMesh->pointL[f.points[0]].pos[iPermutation[1]],
					fScale * pMesh->pointL[f.points[0]].pos[iPermutation[2]]),

				Vector3(
					fScale * pMesh->pointL[f.points[1]].pos[iPermutation[0]],
					fScale * pMesh->pointL[f.points[1]].pos[iPermutation[1]],
					fScale * pMesh->pointL[f.points[1]].pos[iPermutation[2]]),

				Vector3(
					fScale * pMesh->pointL[f.points[2]].pos[iPermutation[0]],
					fScale * pMesh->pointL[f.points[2]].pos[iPermutation[1]],
					fScale * pMesh->pointL[f.points[2]].pos[iPermutation[2]])
			};
			buffer[i*3 + 0] = VectorAndNormal(point[0], normal);
			buffer[i*3 + 1] = VectorAndNormal(point[1], normal);
			buffer[i*3 + 2] = VectorAndNormal(point[2], normal);
	
			ProcessEdge(pairs,
				Edge(buffer[i*3 + 0].v, buffer[i*3 + 1].v, normal), buffer);
			ProcessEdge(pairs,
				Edge(buffer[i*3 + 1].v, buffer[i*3 + 2].v, normal), buffer);
			ProcessEdge(pairs,
				Edge(buffer[i*3 + 2].v, buffer[i*3 + 0].v, normal), buffer);
		}
		if (pairs.size() > 0)
		{
			printf("WARNING: \"%s\" is not a 2-manifold! %d edges left\n",
				pMesh->name, pairs.size());
		}
		fp = fopen(szShadowVolumeFile, "wb");
		assert(fp != NULL);
		Dump(buffer, fp);

		printf("%.1f seconds elapsed. Resulting shadow volume has %d \
			   vertices. Written into %s\n",
			tsw.Update(), buffer.size(), szShadowVolumeFile);

		pShadowVolumeVBO = auto_ptr<VBO>(new VBO(
			(float *)&buffer[0], sizeof(VectorAndNormal), buffer.size()));
		pShadowVolumeVBO->SetNormalData(sizeof(Vector3));
	}
	else
	{
		unsigned int elements;
		float *data;
		if (Read(elements, data, fp))
		{
			pShadowVolumeVBO = auto_ptr<VBO>
				(new VBO(data, sizeof(float) * 6, elements));
			pShadowVolumeVBO->SetNormalData(sizeof(float) * 3);
		}
		else
		{
			printf("File %s is corrupted. Delete it and reload program\n",
				szShadowVolumeFile);
		}
	}
}


bool ShadowVolumeMesh::ProcessEdge(vector<Edge> &pairs, const Edge &e,
								   vector<VectorAndNormal> &buffer)
{
	if (Verbose(VerboseAll))
		printf("Input Edge (%.0f,%.0f,%.0f) - (%.0f,%.0f,%.0f)",
		e.v1.v[0],
		e.v1.v[1],
		e.v1.v[2],
		e.v2.v[0],
		e.v2.v[1],
		e.v2.v[2]);

	vector<Edge>::iterator iter;
	for (iter = pairs.begin(); iter != pairs.end(); iter++)
	{
		if (*iter == e)
		{
			if (Verbose(VerboseAll))
				printf("\n Deleting [(%.0f,%.0f,%.0f) - (%.0f,%.0f,%.0f)] = \
					   [(%.0f,%.0f,%.0f) - (%.0f,%.0f,%.0f)]\n",
					e.v1.v[0],
					e.v1.v[1],
					e.v1.v[2],
					e.v2.v[0],
					e.v2.v[1],
					e.v2.v[2],	
					iter->v1.v[0],
					iter->v1.v[1],
					iter->v1.v[2],
					iter->v2.v[0],
					iter->v2.v[1],
					iter->v2.v[2]);

			// First triangle
			buffer.push_back(VectorAndNormal(e.v1, e.normal));
			buffer.push_back(VectorAndNormal(e.v1, iter->normal));
			buffer.push_back(VectorAndNormal(e.v2, e.normal));
			// Second triangle
			buffer.push_back(VectorAndNormal(e.v2, iter->normal));
			buffer.push_back(VectorAndNormal(e.v2, e.normal));
			buffer.push_back(VectorAndNormal(e.v1, iter->normal));

			pairs.erase(iter);
			return true;
		}
	}
	if (Verbose(VerboseAll))
		printf(" Inserted\n");

	pairs.push_back(e);
	return false;
}

void ShadowVolumeMesh::Dump(vector<VectorAndNormal> &buffer, FILE *fp)
{
	int elements = buffer.size();
	fwrite(&elements, sizeof(int), 1, fp);

	fwrite(&(*buffer.begin()), sizeof(VectorAndNormal), buffer.size(), fp);
	fclose(fp);
}

bool ShadowVolumeMesh::Read(unsigned int &elements, float *&data, FILE *fp)
{
	if (fread(&elements, sizeof(int), 1, fp) != 1)
	{
		fclose(fp);
		return false;
	}
	data = new float[elements * 6];
	if (fread(data, sizeof(VectorAndNormal), elements, fp) != elements)
	{
		delete [] data;
		data = NULL;
		fclose(fp);
		return false;
	}
	fclose(fp);
	return true;
}