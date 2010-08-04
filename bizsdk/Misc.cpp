/*****************************************************************************
 * Filename			Misc.cpp
 * 
 * License			GPLv3
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
#include <assert.h>

#ifdef __linux__
#include <SDL/SDL_image.h>
#else
#include "SDL_image.h"
#endif

static const float u = 1.0f;
static const float z = 0.0f;

/*****************************************************************************
 * Verbosity managemet
 *****************************************************************************/
static VerboseLevel AppVerboseLevel = VerboseInfo;

VerboseLevel GetVerboseLevel() { return AppVerboseLevel; }
void SetVerboseLevel(VerboseLevel level) { AppVerboseLevel = level; }
bool Verbose(VerboseLevel level) { return AppVerboseLevel >= level; }

/*****************************************************************************
 * Loading files
 *****************************************************************************/
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

void DumpMemoryToFile(unsigned char *address, unsigned int size,
					  const char *filename)
{
	FILE *fp = fopen(filename, "wb");
	if (fp)
	{
		fwrite(address, 1, size, fp);
		fclose(fp);
	}
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
    //loadedImage = IMG_Load( filename ); // Note this doesn't load alpha!!

    
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

/*****************************************************************************
 * Simple mathematical utilities
 *****************************************************************************/
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

/*****************************************************************************
 * OpenGL related functions
 *****************************************************************************/
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
	//PrintOpenGLError();	
	return loc;
}

void RenderQuad2D(float x, float y, float w, float h,
	float u0, float v0, float u1, float v1)
{
	//glEnableClientState(GL_VERTEX_ARRAY);	
	//glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	float afAttribs[] = {
		x    , y    , u0, v1,
		x + w, y    , u1, v1,
		x + w, y + h, u1, v0,
		x    , y + h, u0, v0
	};

	glVertexPointer(2, GL_FLOAT, sizeof(float) * 4, afAttribs);
	glTexCoordPointer(2, GL_FLOAT, sizeof(float) * 4, afAttribs + 2);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	//glDisableClientState(GL_VERTEX_ARRAY);	
	//glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

/*****************************************************************************
 * Random functions
 *****************************************************************************/
float RandRange(float min, float max)
{
	float temp = (rand() / (static_cast<float>(RAND_MAX) + 1.0))
		* (max - min) + min;
	return temp;
}

Point3 RandSphere()
{
	float alpha = RandRange(-M_PI, M_PI);
	float beta = RandRange(-M_PI, M_PI);
	return AlphaBetaRotation(alpha, beta) * Point3(0.0, 0.0, 1.0);
}


/*****************************************************************************
 * Mathematical functions dealing planes and matrices
 *****************************************************************************/

void IntersectionLinePlane(Vector3 &intersection, const float *plane,
						   const Vector3 &a, const Vector3 &b)
{
	Vector3 diff = b - a;

	const float den = diff.dot(plane);
	const float num = a.dot(plane);

	intersection = a - diff * (num / den);
}

/*
// Pseudocode for InfinitePlane
if dot(plane, eye) < 0 then plane not visible, return 0
// Calculate 4 vertexes of zfar clipping plane in world coordinates.
(-1,-1), (1,-1), (1,1), (-1,1) * Inv * zfar
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
unsigned int InfinitePlane(Vector3 *poly, const float *plane,
			const Matrix4 &inv, const Vector3 &eye, const float zfar)
{
	Vector3 vplane = Vector3(plane) * plane[3];
	if ((eye + vplane).dot(plane) < 0.0f)
	{
		return 0;
	}

	// row-major version
	Vector3 world[5] = {
		Vector3(-inv.at(0) - inv.at(1) + inv.at(2) + inv.at(3), 
		        -inv.at(4) - inv.at(5) + inv.at(6) + inv.at(7),
		        -inv.at(8) - inv.at(9) + inv.at(10) + inv.at(11)) * zfar,

		Vector3( inv.at(0) - inv.at(1) + inv.at(2) + inv.at(3), 
		         inv.at(4) - inv.at(5) + inv.at(6) + inv.at(7),
		         inv.at(8) - inv.at(9) + inv.at(10) + inv.at(11)) * zfar,

		Vector3( inv.at(0) + inv.at(1) + inv.at(2) + inv.at(3), 
		         inv.at(4) + inv.at(5) + inv.at(6) + inv.at(7),
		         inv.at(8) + inv.at(9) + inv.at(10) + inv.at(11)) * zfar,

		Vector3(-inv.at(0) + inv.at(1) + inv.at(2) + inv.at(3), 
		        -inv.at(4) + inv.at(5) + inv.at(6) + inv.at(7),
		        -inv.at(8) + inv.at(9) + inv.at(10) + inv.at(11)) * zfar,

		Vector3(-inv.at(0) - inv.at(1) + inv.at(2) + inv.at(3), 
		        -inv.at(4) - inv.at(5) + inv.at(6) + inv.at(7),
		        -inv.at(8) - inv.at(9) + inv.at(10) + inv.at(11)) * zfar,

	};

	unsigned int count = 0;
	Vector3 *p = poly;
	bool clip = false;
	for (unsigned int vert = 5; vert; )
	{
		vert--;
		/*
			Check which side of the Plane this corner of the zfar clipping
			plane is on. [A,B,C] of plane equation is the plane normal, D is
			distance from origin; hence [pvPlane->x * -pvPlane->w,
										 pvPlane->y * -pvPlane->w,
										 pvPlane->z * -pvPlane->w]
			is a point on the plane
		*/
		if ((world[vert] + vplane).dot(plane) < 0.0f)
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
	assert(count <= 5);
	assert(count != 2);
	assert(count != 1);
	
	return count;
}


// To be used instead of gluPerspective
// NOTE: For consistency, should use the row major version and call glLoadTransposeMatrixf!!
Matrix4 ProjectionRH(const float fov, const float invAspect, const float znear, const float zfar)
{
	float e = u / tan(0.5f * fov);
	float f1 = -(zfar + znear) / (zfar - znear);
	float f2 = -(0.5f * zfar * znear) / (zfar - znear);

	// row major version
	return Matrix4(
		e, z,          z, z,
		z, e * invAspect, z, z,
		z, z,          f1, f2,
		z, z,          -u, z);
}

// To be used instead of gluPerspective
// NOTE: For consistency, should use the row major version and call glLoadTransposeMatrixf!!
Matrix4 ProjectionRHInfinite(const float fov, const float invAspect, const float znear)
{
	float e = u / tan(0.5f * fov);

	// row major version
	return Matrix4(
		e, z,          z, z,
		z, e * invAspect, z, z,
		z, z,          -u, -2.0f * znear,
		z, z,          -u, z);
}

// NOTE: This is the inverse of the row-major version
Matrix4 InverseProj(const float e, const float a, const float znear, const float zfar)
{
	float f1 = -(zfar + znear) / (zfar - znear);
	float f2 = -(0.5f * zfar * znear) / (zfar - znear);
	return Matrix4(
		u/e,   z,     z,    z,
		  z, a/e,     z,    z,
		  z,   z,     z,   -u,
		  z,   z,  u/f2, f1/f2);
}


// NOTE: This is the inverse of the row-major version
Matrix4 InverseInfProj(const float e, const float a, const float znear)
{
	const float n2 = 2.0f * znear;
	return Matrix4(
		u/e,   z,     z,    z,
		  z, a/e,     z,    z,
		  z,   z,     z,   -u,
		  z,   z, -u/n2, u/n2);
}

Matrix4 InverseProjectionRHInfinite(const float fov, const float aspect, const float znear)
{
	return InverseInfProj(1.0f / tan(0.5f * fov), 1.0f / aspect, znear);
}

Matrix4 InverseProjectionRH(const float fov, const float aspect, const float znear, const float zfar)
{
	return InverseProj(1.0f / tan(0.5f * fov), 1.0f / aspect, znear, zfar);
}

Matrix4 InverseMVP(const Matrix4 &invP, const Vector3 &T, const Matrix4 &R)
{
	return R.Transpose() * Matrix4::Translation(T).Transpose() * invP;
}


Matrix3 AlphaBetaRotation(const float alpha, const float beta)
{
	return Matrix3::RotationX(beta) * Matrix3::RotationY(alpha);
}

/*****************************************************************************
 * Collision detection routines
 *****************************************************************************/
bool CollisionSegmentSphere(const Vector3 &a, const Vector3 &b, const Vector3 &s, const float r)
{
	Vector3 bs = s - b;
	if (bs.Length() < r)
		return true;
	// test with old value (can skip this?)
	Vector3 as = s - a;
	if (as.Length() < r)
		return true;
		
	Vector3 ab = b - a;
	
	float lambda = ab.dot(as) / ab.dot(ab);
	if (lambda <= 0.0f || lambda > 1.0f)
		return false;
		
	return (a + ab * lambda - s).Length() < r;	
}


bool CollisionSphereSphere(const Point3 &a, const Point3 &b, const float r)
{
	return (a - b).Length() < r;
}
