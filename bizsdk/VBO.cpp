/*****************************************************************************
 * Filename			VBO.cpp
 * 
 * License			LGPL
 *
 * Author			Andrea Bizzotto (bizz84@gmail.com)
 *
 * Platform			LinuxX11 / OpenGL
 * 
 * Description		OpenGL Vertex Buffer Object
 *
 *****************************************************************************/

#include "VBO.h"

VBO::VBO(void *data, GLsizei stride, unsigned int count)
	: uiStride(stride), uiCount(count)
{
	glGenBuffersARB(1, &uiVBO);					
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, uiVBO);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, stride * count, data,
		GL_STATIC_DRAW_ARB);

	// Unbind for safety
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
}

VBO::~VBO()
{
	glDeleteBuffersARB(1, &uiVBO);
	for (unsigned int i = 0; i < aEntry.size(); i++)
	{
		delete aEntry[i];
	}
}

void VBO::AddEntry(ArrayFuncPointer pointer, GLint size, GLenum type,
	unsigned int offset)
{
	aEntry.push_back(new VBOEntry(pointer, size, type, offset));
}

void VBO::Bind()
{
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, uiVBO);
	for (unsigned int i = 0; i < aEntry.size(); i++)
	{
		(*aEntry[i]->fnPointer)(aEntry[i]->iSize, aEntry[i]->eType, uiStride,
			aEntry[i]->pOffset);
	}
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
}

void VBO::Bind(ArrayFuncPointer funcPointer, GLint size, GLenum type,
	unsigned int offset)
{
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, uiVBO);
	(*funcPointer)(size, type, uiStride, (void *)offset);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
}

void VBO::Render()
{
	Bind();
	glDrawArrays(GL_TRIANGLES, 0, GetCount());
}





IndexedVBO::IndexedVBO(void *data, GLsizei stride, unsigned int count,
	void *indices, unsigned int elements)
	: VBO(data, stride, count), uiElements(elements)
{
	// Load The Indices
	glGenBuffersARB( 1, &uiIndexVBO );
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, uiIndexVBO);
	glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB,
		elements * sizeof(GL_UNSIGNED_INT), indices, GL_STATIC_DRAW_ARB );	
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
}

void IndexedVBO::Bind()
{
	VBO::Bind();
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, uiIndexVBO);
}	

void IndexedVBO::Unbind()
{
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
}

void IndexedVBO::Render()
{
	Bind();
	glDrawElements(GL_TRIANGLES, GetElements(), GL_UNSIGNED_INT, 0);
	Unbind();
}

