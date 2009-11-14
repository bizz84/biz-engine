/*****************************************************************************
 * Filename			Misc.h
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

#ifndef MISC_H
#define MISC_H

#include <GL/gl.h>
#include <SDL/SDL.h>


enum VerboseLevel { NoVerbose, VerboseInfo, VerboseAll };

VerboseLevel GetVerboseLevel();
void SetVerboseLevel(VerboseLevel level);
bool Verbose(VerboseLevel level);

#define MAX_PATH_LENGTH 256


int LoadFileIntoMemory(const char *filename, char **result);

void FreeFileMemory(char **memory);

bool LoadImage(const char *filename, SDL_Surface *&surface, GLenum &textureFormat, GLint &nOfColors);

void RenderQuad(float x, float y, float width, float height, float u0, float v0, float u1, float v1);

int IntRound(double x);

int NextPowerOfTwo(int x);


#endif
