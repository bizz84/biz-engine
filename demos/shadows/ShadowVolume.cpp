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

ShadowVolume::ShadowVolume(Lib3dsMesh *mesh) : pVBO(NULL)
{
	char szShadowVolumeFile[50];
	sprintf(szShadowVolumeFile, "data/scenes/%s.sv", mesh->name);
	FILE *fp = fopen(szShadowVolumeFile, "rb");
	if (fp == NULL)
	{
		Timer tsw;
		printf("Shadow volume for mesh %s does not exist. Creating it...\n", mesh->name);

		vector<Edge> pairs;
		vector<VectorAndNormal> buffer(mesh->faces * 3);

		// First, create buffer containing initial geometry. This will be a normal GL_TRIANGLES vbo
		unsigned int i;
		for (i = 0; i < mesh->faces; i++)
		{
			Vector3 normal(mesh->faceL[i].normal[permutation[0]],
				mesh->faceL[i].normal[permutation[1]],
				mesh->faceL[i].normal[permutation[2]]);

			Vector3 point[3] = {
				Vector3(mesh->pointL[mesh->faceL[i].points[0]].pos[permutation[0]],
					mesh->pointL[mesh->faceL[i].points[0]].pos[permutation[1]],
					mesh->pointL[mesh->faceL[i].points[0]].pos[permutation[2]]),

				Vector3(mesh->pointL[mesh->faceL[i].points[1]].pos[permutation[0]],
					mesh->pointL[mesh->faceL[i].points[1]].pos[permutation[1]],
					mesh->pointL[mesh->faceL[i].points[1]].pos[permutation[2]]),

				Vector3(mesh->pointL[mesh->faceL[i].points[2]].pos[permutation[0]],
					mesh->pointL[mesh->faceL[i].points[2]].pos[permutation[1]],
					mesh->pointL[mesh->faceL[i].points[2]].pos[permutation[2]])
			};
			buffer[i*3 + 0] = VectorAndNormal(point[0], normal);
			buffer[i*3 + 1] = VectorAndNormal(point[1], normal);
			buffer[i*3 + 2] = VectorAndNormal(point[2], normal);
		}


		for (i = 0; i < mesh->faces; i++)
		{
			ProcessEdge(mesh, pairs, Edge(mesh->faceL[i].points[0], mesh->faceL[i].points[1], i), buffer);
			ProcessEdge(mesh, pairs, Edge(mesh->faceL[i].points[1], mesh->faceL[i].points[2], i), buffer);
			ProcessEdge(mesh, pairs, Edge(mesh->faceL[i].points[2], mesh->faceL[i].points[0], i), buffer);
		}
		fp = fopen(szShadowVolumeFile, "wb");
		assert(fp != NULL);
		Dump(buffer, fp);

		printf("%.1f seconds elapsed. Resulting shadow volume has %d vertices. Written into %s\n",
			tsw.Update(), buffer.size(), szShadowVolumeFile);

		pVBO = new VBO((float *)&buffer[0], sizeof(VectorAndNormal), buffer.size());
		pVBO->SetNormalData(sizeof(Vector3));
	}
	else
	{
		unsigned int elements;
		float *data;
		assert(Read(elements, data, fp));
		pVBO = new VBO(data, sizeof(float) * 6, elements);
		pVBO->SetNormalData(sizeof(float) * 3);
	}
}

ShadowVolume::~ShadowVolume()
{
	delete pVBO;
}

bool ShadowVolume::ProcessEdge(Lib3dsMesh *mesh, vector<Edge> &pairs, const Edge &e, vector<VectorAndNormal> &buffer)
{
	vector<Edge>::iterator iter;
	for (iter = pairs.begin(); iter != pairs.end(); iter++)
	{
		if (*iter == e)
		{
			// Normal of original edge
			Vector3 n1(mesh->faceL[iter->face].normal[permutation[0]],
				mesh->faceL[iter->face].normal[permutation[1]],
				mesh->faceL[iter->face].normal[permutation[2]]);

			// Normal of matching edge
			Vector3 n2(mesh->faceL[e.face].normal[permutation[0]],
				mesh->faceL[e.face].normal[permutation[1]],
				mesh->faceL[e.face].normal[permutation[2]]);

			// Vector 1 of the edge
			Vector3 v1(mesh->pointL[iter->v1].pos[permutation[0]],
				mesh->pointL[iter->v1].pos[permutation[1]],
				mesh->pointL[iter->v1].pos[permutation[2]]);

			// Vector 2 of the edge
			Vector3 v2(mesh->pointL[e.v1].pos[permutation[0]],
				mesh->pointL[e.v1].pos[permutation[1]],
				mesh->pointL[e.v1].pos[permutation[2]]);

			// First triangle
			buffer.push_back(VectorAndNormal(v1, n2));
			buffer.push_back(VectorAndNormal(v2, n1));
			buffer.push_back(VectorAndNormal(v1, n1));
			// Second triangle
			buffer.push_back(VectorAndNormal(v2, n2));
			buffer.push_back(VectorAndNormal(v1, n2));
			buffer.push_back(VectorAndNormal(v2, n1));

			pairs.erase(iter);
			return true;
		}
	}
	pairs.push_back(e);
	return false;
}

void ShadowVolume::Dump(vector<VectorAndNormal> &buffer, FILE *fp)
{
	int elements = buffer.size();
	fwrite(&elements, sizeof(int), 1, fp);

	fwrite(&(*buffer.begin()), sizeof(VectorAndNormal), buffer.size(), fp);
	fclose(fp);
}

bool ShadowVolume::Read(unsigned int &elements, float *&data, FILE *fp)
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
