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

using namespace std;

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
protected:
	bool init;
	GLuint uiProgram;

	static const float VertexAttrib[3 * 8];
	static const int ElementAttrib[24];

	auto_ptr<IndexedVBO> pVBOCube;

	virtual bool ShaderSetup();

	SkyBox();
	virtual ~SkyBox() { }
public:
	static SkyBox &Instance();
	bool Init();
	void Render() const;
};

/*****************************************************************************
 * SkyBoxTransition class
 * Can blend between two cubemaps
 *****************************************************************************/
class SkyBoxTransition : public SkyBox
{
protected:
	GLint locMix;
	virtual bool ShaderSetup();
	
	SkyBoxTransition() : locMix(~0) { }
public:
	static SkyBoxTransition &Instance();
	virtual void SetUniforms() { }

	void Render(const float mix) const;
};

#endif

