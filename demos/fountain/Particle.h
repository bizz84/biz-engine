/*****************************************************************************
 * Filename			NBody.h
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
#ifndef _PARTICLE_H_
#define _PARTICLE_H_

#if defined(__APPLE__) || defined(__MACOSX)
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif

#include "Extensions.h"
#include "VBO.h"

class ParticleVBO : public VBO
{
protected:

public:
	ParticleVBO(void *data, unsigned int stride, unsigned int count);

	void SetCount(unsigned int count) { uiCount = count; }
	unsigned int GetCount() { return uiCount; }
};


#endif