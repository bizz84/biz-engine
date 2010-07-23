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

#include "WeaponRenderer.h"
#include "Misc.h"

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

WeaponRenderer::WeaponRenderer()
	: pTetraVBO(NULL)
{
	// Initialize vbo used for tetrahedron
	pTetraVBO = new IndexedVBO((void *)TetraVertices, sizeof(float) * 5, 4,
	                           (void *)TetraIndices, 12);
	pTetraVBO->SetTexCoordData(sizeof(float) * 3);

	assert(LoadShaders(Shaders, NUM_PROGRAMS));
}

WeaponRenderer::~WeaponRenderer()
{
	delete pTetraVBO;
}


void WeaponRenderer::Render(const list<Bullet *> &bullets) const
{
	float yellow[] = { 1.0f, 1.0f, 0.0f, 1.0f };
	GLuint shader = Program(P_LOOKUP_COLOR);
	glUseProgram(shader);
	glUniform4fv(GetUniLoc(shader, "Color"), 1, yellow);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

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

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}
