/*****************************************************************************
 * Filename			Timer.h
 * 
 * License			LGPL
 *
 * Author			Andrea Bizzotto (bizz84@gmail.com)
 *
 * Platform			LinuxX11 / OpenGL
 * 
 * Description		High resolution timer
 *
 *****************************************************************************/

#ifndef _SKYBOX_H_
#define _SKYBOX_H_

#include "Extensions.h"
#include "VBO.h"

class SkyBox
{
	bool init;
	float fScale;

	GLuint uiSkybox;
	GLuint uiProgram;

	static const float VertexAttrib[3 * 8];
	static const int ElementAttrib[24];
protected:
	IndexedVBO *vboCube;

	SkyBox();
	~SkyBox();
public:
	static SkyBox &Instance();
	bool Init(const char *textures[], const float scale = 1.0f);
	void Render(const float alpha, const float beta);
};

class SkyBoxTexture
{

};

class SkyBoxShader
{

};

#endif

