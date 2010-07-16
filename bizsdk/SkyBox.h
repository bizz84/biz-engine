/*****************************************************************************
 * Filename			Timer.h
 * 
 * License			LGPL
 *
 * Author			Andrea Bizzotto (bizz84@gmail.com)
 *
 * Platform			LinuxX11 / OpenGL
 * 
 * Description		CubeMap and SkyBox classes
 *
 *****************************************************************************/

#ifndef _SKYBOX_H_
#define _SKYBOX_H_

#include "Extensions.h"
#include "VBO.h"

/*****************************************************************************
 * CubeMap class (loads GL_TEXTURE_CUBE_MAP from 6 textures
 *****************************************************************************/
class CubeMap
{
	GLuint uiCubeMap;
public:
	bool Init(const char *textures[]);
	const GLuint Get() const { return uiCubeMap; }
};

/*****************************************************************************
 * SkyBox class (loads GL_TEXTURE_CUBE_MAP from 6 textures
 * SkyBox is implemented as a singleton
 *****************************************************************************/
class SkyBox
{
	bool init;
	GLuint uiProgram;

	static const float VertexAttrib[3 * 8];
	static const int ElementAttrib[24];
protected:
	IndexedVBO *vboCube;

	SkyBox();
	~SkyBox();
public:
	static SkyBox &Instance();
	bool Init();
	void Render(const CubeMap &cubemap) const;
};

#endif

