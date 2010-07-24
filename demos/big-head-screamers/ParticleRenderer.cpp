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

#include "ExplosionRenderer.h"
#include "Misc.h"
#include "Bullet.h" // Bullet part of it
#include "ParticleEmitter.h"
#include "GLResourceManager.h"

#include <assert.h>


static const char *Shaders[] = {
	"data/shaders/Particle.vert", "data/shaders/Particle.frag",
};

ExplosionRenderer::ExplosionRenderer()
{
	assert(LoadShaders(Shaders, NUM_PROGRAMS));
}


void ExplosionRenderer::Render(const list<ParticleEmitter *> &explosions) const
{
	GLuint shader = Program(P_PARTICLE);
	glUseProgram(shader);

	glEnableClientState(GL_VERTEX_ARRAY);
	list<ParticleEmitter *>::const_iterator iter;
	for (iter = explosions.begin(); iter != explosions.end(); iter++)
	{
		(*iter)->Render();
	}
	glDisableClientState(GL_VERTEX_ARRAY);
}