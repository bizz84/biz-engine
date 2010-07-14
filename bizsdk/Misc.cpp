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
#define _USE_MATH_DEFINES
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



/*
//
if dot(plane, eye) < 0 then plane not visible, return 0
// Calculate 4 vertexes of far clipping plane in world coordinates.
(-1,-1), (1,-1), (1,1), (-1,1) * Inv * far
// Loop
clip = false
for (vert = 5; vert; vert--)
    if (dot(corner(vert), plane < 0) // no clipping
        if clip
            clip = false
            *V++ = Intersect(plane, P[vert+1], P[vert])
        if !vert return
        *V++ = Intersect(plane, eye, P[vert])
    else
        if clip
            continue // will be handled later
        clip = true
        if (vert != 4)
            *V++ = Intersect(plane, V[vert+1], V[vert])
    endif
end
return V.size

*/
void IntersectionLinePlane(Vector3 &intersection, const float *plane, const Vector3 &a, const Vector3 &b)
{
	Vector3 diff = b - a;

	const float den = diff.dot(plane);
	const float num = a.dot(plane);

	intersection = a - diff * (num / den);
}

unsigned int InfinitePlane(Vector3 *poly, const float *plane, const Matrix4 &inv, const Vector3 &eye, const float far)
{
	//if ((eye[0] + plane[0] * plane[3]) * plane[0] +
	//	(eye[1] + plane[1] * plane[3]) * plane[1] +
	//	(eye[2] + plane[2] * plane[3]) * plane[2] < 0.0f)
	Vector3 vplane = Vector3(plane) * plane[3];
	if ((eye + vplane).dot(plane) < 0.0f)
	{
		return 0;
	}
	Vector3 world0[5] = {
		Vector3(-inv.at(0) - inv.at(4) + inv.at(8) + inv.at(12), 
		        -inv.at(1) - inv.at(5) + inv.at(9) + inv.at(13),
		        -inv.at(2) - inv.at(6) + inv.at(10) + inv.at(14)) * far,

		Vector3( inv.at(0) - inv.at(4) + inv.at(8) + inv.at(12), 
		         inv.at(1) - inv.at(5) + inv.at(9) + inv.at(13),
		         inv.at(2) - inv.at(6) + inv.at(10) + inv.at(14)) * far,

		Vector3( inv.at(0) + inv.at(4) + inv.at(8) + inv.at(12), 
		         inv.at(1) + inv.at(5) + inv.at(9) + inv.at(13),
		         inv.at(2) + inv.at(6) + inv.at(10) + inv.at(14)) * far,

		Vector3(-inv.at(0) + inv.at(4) + inv.at(8) + inv.at(12), 
		        -inv.at(1) + inv.at(5) + inv.at(9) + inv.at(13),
		        -inv.at(2) + inv.at(6) + inv.at(10) + inv.at(14)) * far,

		Vector3(-inv.at(0) - inv.at(4) + inv.at(8) + inv.at(12), 
		        -inv.at(1) - inv.at(5) + inv.at(9) + inv.at(13),
		        -inv.at(2) - inv.at(6) + inv.at(10) + inv.at(14)) * far,
	};

	Vector3 world[5] = {
		Vector3(-inv[0][0] - inv[0][1] + inv[0][2] + inv[0][3], 
		        -inv[1][0] - inv[1][1] + inv[1][2] + inv[1][3],
		        -inv[2][0] - inv[2][1] + inv[2][2] + inv[2][3]) * far,

		Vector3( inv[0][0] - inv[0][1] + inv[0][2] + inv[0][3], 
		         inv[1][0] - inv[1][1] + inv[1][2] + inv[1][3],
		         inv[2][0] - inv[2][1] + inv[2][2] + inv[2][3]) * far,

		Vector3( inv[0][0] + inv[0][1] + inv[0][2] + inv[0][3], 
		         inv[1][0] + inv[1][1] + inv[1][2] + inv[1][3],
		         inv[2][0] + inv[2][1] + inv[2][2] + inv[2][3]) * far,

		Vector3(-inv[0][0] + inv[0][1] + inv[0][2] + inv[0][3], 
		        -inv[1][0] + inv[1][1] + inv[1][2] + inv[1][3],
		        -inv[2][0] + inv[2][1] + inv[2][2] + inv[2][3]) * far,

		Vector3(-inv[0][0] - inv[0][1] + inv[0][2] + inv[0][3], 
		        -inv[1][0] - inv[1][1] + inv[1][2] + inv[1][3],
		        -inv[2][0] - inv[2][1] + inv[2][2] + inv[2][3]) * far
	};
	Vector3 world2[5] = {
		Vector3(-inv[0][0] - inv[1][0] + inv[2][0] + inv[3][0], 
		        -inv[0][1] - inv[1][1] + inv[2][1] + inv[3][1],
		        -inv[0][2] - inv[1][2] + inv[2][2] + inv[3][2]) * far,

		Vector3( inv[0][0] - inv[1][0] + inv[2][0] + inv[3][0], 
		         inv[0][1] - inv[1][1] + inv[2][1] + inv[3][1],
		         inv[0][2] - inv[1][2] + inv[2][2] + inv[3][2]) * far,

		Vector3( inv[0][0] + inv[1][0] + inv[2][0] + inv[3][0], 
		         inv[0][1] + inv[1][1] + inv[2][1] + inv[3][1],
		         inv[0][2] + inv[1][2] + inv[2][2] + inv[3][2]) * far,

		Vector3(-inv[0][0] + inv[1][0] + inv[2][0] + inv[3][0], 
		        -inv[0][1] + inv[1][1] + inv[2][1] + inv[3][1],
		        -inv[0][2] + inv[1][2] + inv[2][2] + inv[3][2]) * far,

		Vector3(-inv[0][0] - inv[1][0] + inv[2][0] + inv[3][0], 
		        -inv[0][1] - inv[1][1] + inv[2][1] + inv[3][1],
		        -inv[0][2] - inv[1][2] + inv[2][2] + inv[3][2]) * far
	};

	float dot;
	unsigned int count = 0;
	Vector3 *p = poly;
	bool clip = false;
	for (unsigned int vert = 5; vert; )
	{
		vert--;
		/*
			Check which side of the Plane this corner of the far clipping
			plane is on. [A,B,C] of plane equation is the plane normal, D is
			distance from origin; hence [pvPlane->x * -pvPlane->w,
										 pvPlane->y * -pvPlane->w,
										 pvPlane->z * -pvPlane->w]
			is a point on the plane
		*/
		dot = (world[vert] + vplane).dot(plane);
		//dot = (world[vert][0] + plane[0] * plane[3]) * plane[0] +
		//	  (world[vert][1] + plane[1] * plane[3]) * plane[1] +
		//	  (world[vert][2] + plane[2] * plane[3]) * plane[2];

		if (dot < 0.0f)
		{
			if (clip)
			{
				clip = false;
				IntersectionLinePlane(*p, plane, world[vert+1], world[vert]);
				p++;
				count++;
			}
			if (!vert)
			{
				break;
			}
			IntersectionLinePlane(*p, plane, eye, world[vert]);
			p++;
			count++;
		}
		else
		{
			if (clip)
				continue;

			clip = true;

			if (vert != 4)
			{
				IntersectionLinePlane(*p, plane, world[vert+1], world[vert]);
				p++;
				count++;
			}
		}

	}
	
	return count;
}