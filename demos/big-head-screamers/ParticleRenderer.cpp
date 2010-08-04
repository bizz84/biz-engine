/*****************************************************************************
 * Filename			Weapon.cpp
 * 
 * License			GPLv3
 *
 * Author			Andrea Bizzotto (bizz84@gmail.com)
 *
 * Platform			LinuxX11 / OpenGL
 * 
 * Description		Weapon
 *
 *****************************************************************************/

#include "ParticleRenderer.h"
#include "ParticleEmitter.h"

#include <assert.h>

static const char *Shaders[] = {
	"data/shaders/Particle.vert", "data/shaders/Particle.frag",
};

ParticleRenderer::ParticleRenderer()
{
	assert(LoadShaders(Shaders, NUM_PROGRAMS));
}


void ParticleRenderer::Render(const ptr_list<ParticleEmitter> &particles) const
{
	GLuint shader = Program(P_PARTICLE);
	glUseProgram(shader);

	//glEnableClientState(GL_VERTEX_ARRAY);
	ptr_list<ParticleEmitter>::const_iterator iter;
	for (iter = particles.begin(); iter != particles.end(); iter++)
	{
		iter->Render();
	}
	//glDisableClientState(GL_VERTEX_ARRAY);
}