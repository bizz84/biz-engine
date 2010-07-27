/*****************************************************************************
 * Filename			Weapon.cpp
 * 
 * License			LGPL
 *
 * Author			Andrea Bizzotto (bizz84@gmail.com)
 *
 * Platform			LinuxX11 / OpenGL
 * 
 * Description		Weapon
 *
 *****************************************************************************/

#include "TetraRenderer.h"
#include "Misc.h"
#include "Bullet.h" // Bullet part of it
#include "GLResourceManager.h"

#include <assert.h>

// Object representing a tetrahedron
static const float TetraVertices[20] = {
	 0.0f    ,  1.0f    ,  0.0     , 0.5f, 1.0f,
	 0.0f    , -0.33333f,  0.94281f, 0.5f, 0.0f, 
	-0.81650f, -0.33333f, -0.47140f, 0.0f, 0.0f,
	 0.81650f, -0.33333f, -0.47140f, 1.0f, 0.0f,
};

static const int TetraIndices[12] = {
	0, 1, 2,
	0, 2, 3,
	0, 3, 1,
	1, 2, 3
};

static const float AmmoSize = 5.0f;

static const char *Shaders[] = {
	"data/shaders/LookupColor.vert", "data/shaders/LookupColor.frag",
};

TetraRenderer::TetraRenderer()
	: pTetraVBO(NULL)
{
	assert(LoadShaders(Shaders, NUM_PROGRAMS));

	// Initialize vbo used for tetrahedron
	pTetraVBO = new IndexedVBO((void *)TetraVertices, sizeof(float) * 5, 4,
	                           (void *)TetraIndices, 12);
	pTetraVBO->SetTexCoordData(sizeof(float) * 3);
}

TetraRenderer::~TetraRenderer()
{
	delete pTetraVBO;
}


void TetraRenderer::Render(const list<Bullet *> &bullets) const
{
	// TODO: This is pre-render (factor out as the function is templatized)
	float color[] = { 1.0f, 1.0f, 0.0f, 1.0f };
	GLuint shader = Program(P_LOOKUP_COLOR);
	glUseProgram(shader);
	glUniform4fv(GetUniLoc(shader, "Color"), 1, color);

	//glEnableClientState(GL_VERTEX_ARRAY);
	//glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	list<Bullet *>::const_iterator iter;
	for (iter = bullets.begin(); iter != bullets.end(); iter++)
	{
		glPushMatrix();
		const Point3 &pos = (*iter)->GetPosition();
		glTranslatef(pos[0], pos[1], pos[2]);
		glScalef(AmmoSize, AmmoSize, AmmoSize);
		pTetraVBO->Render(GL_TRIANGLES);
		glPopMatrix();
	}

	// TODO: This is post-render (factor out as the function is templatized)
	//glDisableClientState(GL_NORMAL_ARRAY);
	//glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

