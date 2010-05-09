/*****************************************************************************
 * Filename			NBody.cpp
 * 
 * License			LGPL
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



ParticleVBO::ParticleVBO(void *data, unsigned int count)
: VBO(data, sizeof(cl_float4), count)
{
	uiVertexSize = 4;
}