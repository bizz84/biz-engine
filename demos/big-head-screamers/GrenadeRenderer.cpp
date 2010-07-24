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

#include "GrenadeRenderer.h"
#include "Misc.h"
#include "Bullet.h" // Bullet part of it
#include "GLResourceManager.h"
#include "Mesh.h"

#include <assert.h>


static const float AmmoSize = 1.5f;

static const char *Shaders[] = {
	"data/shaders/Grenade.vert", "data/shaders/Grenade.frag",
};

GrenadeRenderer::GrenadeRenderer()
{
	pMesh[0] = pMesh[1] = NULL;

	assert(LoadShaders(Shaders, NUM_PROGRAMS));


	GLResourceManager &loader = GLResourceManager::Instance();

	unsigned int index;
	assert(loader.Load3DSFile("data/scenes/grenade.3ds", index));
	int permutation[] = { 0, 1, 2 };
	pMesh[0] = new Mesh(loader.FindMesh(index, "Sphere"), AmmoSize, false, permutation);
	pMesh[1] = new Mesh(loader.FindMesh(index, "Sphere.001"), AmmoSize, false, permutation);
}

GrenadeRenderer::~GrenadeRenderer()
{
	delete pMesh[0];
	delete pMesh[1];
}


void GrenadeRenderer::Render(const list<Bullet *> &bullets) const
{
	// TODO: This is pre-render (factor out as the function is templatized)
	float color[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	GLuint shader = Program(P_GRENADE);
	glUseProgram(shader);
	glUniform4fv(GetUniLoc(shader, "Color"), 1, color);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	list<Bullet *>::const_iterator iter;
	for (iter = bullets.begin(); iter != bullets.end(); iter++)
	{
		glPushMatrix();
		const Point3 &pos = (*iter)->GetPosition();
		glTranslatef(pos[0], pos[1], pos[2]);
		glRotatef(-((Grenade *)*iter)->GetAngleY() * 180.0f / M_PI, 0.0f, 1.0f, 0.0f);
		glRotatef(-((Grenade *)*iter)->GetAngleX() * 180.0f / M_PI, 1.0f, 0.0f, 0.0f);
		glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
		pMesh[0]->GetVBO()->Render(GL_TRIANGLES);
		pMesh[1]->GetVBO()->Render(GL_TRIANGLES);
		glPopMatrix();
	}

	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}
