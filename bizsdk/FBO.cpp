/*****************************************************************************
 * Filename			FBO.cpp
 * 
 * License			LGPL
 *
 * Author			Andrea Bizzotto (bizz84@gmail.com)
 *
 * Platform			LinuxX11 / OpenGL
 * 
 * Description		Frame buffer class
 *
 *****************************************************************************/
 
#include "FBO.h"
#include <assert.h>

FBO::FBO(GLenum internalFormat, GLenum format, unsigned int width,
	unsigned int height, bool attachDepthBuffer/* = true*/)
	: uiWidth(width), uiHeight(height),
	uiFBO(~0), uiOriginalFBO(~0), uiTexture(~0), uiDepthBuffer(~0)
{
	// Save viewport
	//glGetIntegerv( GL_VIEWPORT, aiPrevViewport );

	// Texture setup
	// Generate texture to be used with the frame buffer
	glGenTextures(1, &uiTexture);

	// Binds this texture handle so we can load the data into it
	glBindTexture(GL_TEXTURE_2D, uiTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, uiWidth,
		uiHeight, 0, internalFormat, format, 0);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Framebuffer object setup
	glGenFramebuffers(1, &uiFBO);
	
	// Store currently bound framebuffer, will be restored later 
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &uiOriginalFBO);
	
	// Bind created buffer
	glBindFramebuffer(GL_FRAMEBUFFER, uiFBO);
		
	// Attach a texture
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
		uiTexture, 0);
		
	if (attachDepthBuffer)
	{
		glGenRenderbuffers(1, &uiDepthBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, uiDepthBuffer);

		// Tell GL that the new render buffer is a depth buffer
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16,
			width, height);

		// Now we have our depth buffer attach it to our frame buffer object.
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
			GL_RENDERBUFFER, uiDepthBuffer);
	}
	assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
	// Restore previous framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, uiOriginalFBO);	
}

FBO::~FBO()
{
	// Delete texture
	glDeleteTextures(1, &uiTexture);

	// Delete buffer objects
	glDeleteFramebuffers(1, &uiFBO);

	// Use the fact that uiDepthBuffer is set to ~0 in the constructor to
	// determine if this is used
	if (uiDepthBuffer != ~0)
	{
		// Delete our depth buffer render buffers
		glDeleteRenderbuffers(1, &uiDepthBuffer);
	}
}

bool FBO::BindBuffer() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, uiFBO);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
	{
		//glViewport(0, 0, uiWidth, uiHeight);
		return true;
	}
	return false;
}

void FBO::UnbindBuffer() const
{
	// TODO: unbind only if currently bound?
	glBindFramebuffer(GL_FRAMEBUFFER, uiOriginalFBO);
	//glViewport(aiPrevViewport[0], aiPrevViewport[1],
	//           aiPrevViewport[2], aiPrevViewport[3]);
}

void FBO::BindTexture() const
{
	glBindTexture(GL_TEXTURE_2D, uiTexture);
}

void FBO::UnbindTexture() const
{
	glBindTexture(GL_TEXTURE_2D, 0);
}


