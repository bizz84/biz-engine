/*****************************************************************************
 * Filename			Extensions.h
 * 
 * License			LGPL
 *
 * Author			Andrea Bizzotto (bizz84@gmail.com)
 *
 * Platform			LinuxX11 / OpenGL
 * 
 * Description		Utility functions
 *
 *****************************************************************************/

#ifndef EXTENSIONS_H
#define EXTENSIONS_H

#ifndef GL_GLEXT_PROTOTYPES

#include <GL/glew.h>

#else

#include <SDL/SDL_opengl.h>

// Define extensions here
/*#define glCreateShader glCreateShaderObjectARB
#define glGenBuffers glGenBuffersARB
#define glBindBuffer glBindBufferARB
#define glBufferData glBufferDataARB
#define glDeleteBuffers glDeleteBuffersARB*/

#endif

#endif


