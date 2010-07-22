/*****************************************************************************
 * Filename			Weapon.h
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
#ifndef _PROGRAM_ARRAY_H_
#define _PROGRAM_ARRAY_H_

#include <stdio.h>
#include "Extensions.h"

class ProgramArray
{
private:
	GLuint *uiProgram;
protected:
	GLuint Program(int index) const { return uiProgram[index]; }
	bool LoadShaders(const char *Shaders[], const unsigned int n);
public:
	ProgramArray() : uiProgram(NULL) { }
	virtual ~ProgramArray();
	
};

#endif
