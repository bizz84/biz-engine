/*****************************************************************************
 * Filename			Misc.cpp
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

#include "Misc.h"
#include <stdio.h>
#include <malloc.h>
#include <math.h>

#include <stdio.h>

const int permutation[3] = { 1, 2, 0 };

static VerboseLevel AppVerboseLevel = VerboseInfo;

VerboseLevel GetVerboseLevel() { return AppVerboseLevel; }
void SetVerboseLevel(VerboseLevel level) { AppVerboseLevel = level; }
bool Verbose(VerboseLevel level) { return AppVerboseLevel >= level; }


int LoadFileIntoMemory(const char *filename, char **result)
{
	int size = 0;
	FILE *f = fopen(filename, "rb");
	if (f == NULL) 
	{
		*result = NULL;
		return -1; // -1 means file opening fail
	}
	fseek(f, 0, SEEK_END);
	size = ftell(f);
	fseek(f, 0, SEEK_SET);
	*result = (char *)malloc(size+1);
	if (size != (int)fread(*result, sizeof(char), size, f))
	{
		free(*result);
		return -2; // -2 means file reading fail
	}
	fclose(f);
	(*result)[size] = 0;
	return size;
}

void FreeFileMemory(char **memory)
{
	free(*memory);
}



bool LoadImage(const char *filename, SDL_Surface *&surface,
	GLenum &textureFormat, GLint &nOfColors)
{
    //Temporary storage for the image that's loaded
    SDL_Surface* loadedImage = NULL;
    
    //Load the image
    loadedImage = SDL_LoadBMP( filename );
    
    //If nothing went wrong in loading the image
    if( loadedImage != NULL )
    {
        //Create an optimized image
        surface = SDL_DisplayFormat( loadedImage );
        
        //Free the old image
        SDL_FreeSurface( loadedImage );

		if ( surface != NULL )
		{
		    // get the number of channels in the SDL surface
			nOfColors = surface->format->BytesPerPixel;
			if (nOfColors == 4)     // contains an alpha channel
			{
		        if (surface->format->Rmask == 0x000000ff)
		                textureFormat = GL_RGBA;
		        else
		                textureFormat = GL_BGRA;
			}
			else if (nOfColors == 3)     // no alpha channel
			{
		        if (surface->format->Rmask == 0x000000ff)
		                textureFormat = GL_RGB;
		        else
		                textureFormat = GL_BGR;
			}
			else
			{
		        printf("warning: the image is not truecolor..  "
					"this will probably break\n");
		        // this error should not go unhandled
			}
			return true;
		}
		else
		{
			printf("SDL could not load %s: %s\n", filename, SDL_GetError());
			return false;
		}
    }
    return false;
}


void RenderQuad2D(float x, float y, float w, float h,
	float u0, float v0, float u1, float v1)
{
	glEnableClientState(GL_VERTEX_ARRAY);	
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	float afAttribs[] = {
		x    , y    , u0, v1,
		x + w, y    , u1, v1,
		x + w, y + h, u1, v0,
		x    , y + h, u0, v0
	};

	glVertexPointer(2, GL_FLOAT, sizeof(float) * 4, afAttribs);
	glTexCoordPointer(2, GL_FLOAT, sizeof(float) * 4, afAttribs + 2);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glDisableClientState(GL_VERTEX_ARRAY);	
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

int IntRound(double x)
{
	return (int)(x + 0.5);
}

int NextPowerOfTwo(int x)
{
	double logbase2 = log((float)x) / log(2.0f);
	return IntRound(pow(2,ceil(logbase2)));
}


float Inertia(float t, float tau)
{
	if (t <= 0.001f)
		return 0.0f;

	return exp(-tau * t);
}

void PrintOpenGLError()
{
	GLenum err = glGetError();
	if (Verbose(VerboseAll))
	{
		if (err != GL_NO_ERROR)
		{
			printf("GL error %d\n", err);
		}
	}
}

GLint GetUniLoc(GLuint program, const GLchar *name)
{
	GLint loc = glGetUniformLocation(program, name);
	if (Verbose(VerboseAll))
	{
		if (loc == -1)
			printf("No such uniform named \"%s\"\n", name);
	}
	PrintOpenGLError();	
	return loc;
}


float RandRange(float min, float max)
{
	float temp = (rand() / (static_cast<float>(RAND_MAX) + 1.0))
		* (max - min) + min;
	return temp;

	//double val = (double)(rand() % 1073741824);
	//return min + (val * float((min - max) / 1073741824.0));
}
