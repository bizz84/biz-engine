/*****************************************************************************
 * Filename			FBO.h
 * 
 * License			GPLv3
 *
 * Author			Andrea Bizzotto (bizz84@gmail.com)
 *
 * Platform			LinuxX11 / OpenGL
 * 
 * Description		Frame buffer class
 *
 *****************************************************************************/
 
#ifndef _FBO_H_
#define _FBO_H_

#include "Extensions.h"

/*****************************************************************************
 * FBO class (frame buffer object)
 *****************************************************************************/

class FBO
{
	GLsizei uiWidth, uiHeight;
	GLuint uiFBO;
	GLint uiOriginalFBO;
	GLuint uiTexture;
	GLuint uiDepthBuffer;
	
	//GLint aiPrevViewport[4];
public:
	FBO(GLenum internalFormat, GLenum format, unsigned int width,
		unsigned int height, bool attachDepthBuffer = true);
	~FBO();

	// The follow methods change only the GL state so they are const
	// on the CPU side
	bool BindBuffer() const;	
	void UnbindBuffer() const;
	
	void BindTexture() const;
	void UnbindTexture() const;
};

#endif

