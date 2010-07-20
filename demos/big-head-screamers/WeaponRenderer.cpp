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

WeaponRenderer::WeaponRenderer()
	: pTetraVBO(NULL)
{
	// Initialize vbo used for tetrahedron
	pTetraVBO = new IndexedVBO((void *)TetraVertices, sizeof(float) * 5, 4,
	                           (void *)TetraIndices, 12);
	pTetraVBO->SetTexCoordData(sizeof(float) * 3);
}

WeaponRenderer::~WeaponRenderer()
{
	delete pTetraVBO;
}


void WeaponRenderer::Render(const WeaponSystem *ws) const
{
	const list<Bullet *> &bullets = ws->GetBullets();
	list<Bullet *>::const_iterator iter;
	for (iter = bullets.begin(); iter != bullets.end(); iter++)
	{
		glPushMatrix();
		const Point3 &pos = (*iter)->GetPosition();
		glTranslatef(pos[0], pos[1], pos[2]);
		glScalef(10.0f, 10.0f, 10.0f);
		pTetraVBO->Render(GL_TRIANGLES);
		glPopMatrix();
	}
}
