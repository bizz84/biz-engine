/*****************************************************************************
 * Filename			Matrix.cpp
 * 
 * License			GPLv3
 *
 * Author			Andrea Bizzotto (bizz84@gmail.com)
 *
 * Platform			LinuxX11 / OpenGL
 * 
 * Description		Matrix operations
 *
 *****************************************************************************/

#include "Vector.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>

/*****************************************************************************
 * Vector2 implementation
 *****************************************************************************/
Vector2::Vector2()
{
	s[0] = s[1] = 0.0f;
}

Vector2::Vector2(const float *f)
{
	s[0] = f[0];
	s[1] = f[1];
}
Vector2::Vector2(const float x, const float y)
{
	s[0] = x;
	s[1] = y;
}

/*****************************************************************************
 * Vector3 implementation
 *****************************************************************************/
Vector3::Vector3()
{
	s[0] = s[1] = s[2] = 0.0f;
}

Vector3::Vector3(const float *f)
{
	s[0] = f[0];
	s[1] = f[1];
	s[2] = f[2];
}

Vector3::Vector3(const float x, const float y, const float z)
{
	s[0] = x;
	s[1] = y;
	s[2] = z;
}


/*****************************************************************************
 * Vector4 implementation
 *****************************************************************************/

Vector4::Vector4()
{
	s[0] = s[1] = s[2] = s[3] = 0.0f;
}

Vector4::Vector4(const float *f)
{
	float *p = s;
	*p++ = *f++;
	*p++ = *f++;
	*p++ = *f++;
	*p++ = *f++;
}
Vector4::Vector4(const float x, const float y, const float z, const float w)
{
	float *p = s;
	*p++ = x;
	*p++ = y;
	*p++ = z;
	*p++ = w;

}
