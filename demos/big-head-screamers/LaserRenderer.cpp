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

#include "LaserRenderer.h"
#include "Misc.h"
#include "Bullet.h" // Bullet part of it
#include "GLResourceManager.h"
#include "Mesh.h"

#include <assert.h>


static const char *Shaders[] = {
	"data/shaders/Laser.vert", "data/shaders/Laser.frag",
};

LaserRenderer::LaserRenderer()
{
	assert(LoadShaders(Shaders, NUM_PROGRAMS));

}

void LaserRenderer::Render(const list<Bullet *> &bullets) const
{
	// TODO: implement
}
