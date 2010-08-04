/*****************************************************************************
 * Filename			NBody.cpp
 * 
 * License			GPLv3
 *
 * Author			Andrea Bizzotto (bizz84@gmail.com)
 *
 * Platform			LinuxX11 / OpenGL
 * 
 * Description		NBody Simulation
 *
 *****************************************************************************/

#include "Particle.h"


#include <cstdio>
#include <cstdlib>


#include <CL/cl.hpp>


ParticleVBO::ParticleVBO(void *data, unsigned int stride, unsigned int count)
: VBO(data, stride, count)
{
	uiVertexSize = 4;
}