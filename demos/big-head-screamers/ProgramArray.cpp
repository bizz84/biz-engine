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

#include "ProgramArray.h"
#include "GLResourceManager.h"

ProgramArray::~ProgramArray()
{
	delete [] uiProgram;
}


bool ProgramArray::LoadShaders(const char *Shaders[], const unsigned int n)
{
	// Clear errors
	glGetError();
	uiProgram = new GLuint[n];
	GLResourceManager &loader = GLResourceManager::Instance();
	for (unsigned int i = 0; i < n; i++)
	{
		if (!loader.LoadShaderFromFile(Shaders[i<<1], Shaders[(i<<1)+1],
			uiProgram[i]))
			return false;
	}
	return true;
}
