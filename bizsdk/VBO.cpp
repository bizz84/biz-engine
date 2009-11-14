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


//void glVertexPointer(	GLint  	size, 
 //	GLenum  	type, 
 	//GLsizei  	stride, 
 	//const GLvoid *  	pointer);

ElementVBO::ElementVBO(float *data, unsigned int *indices, unsigned int vertexes, unsigned int elements)
	: uiVertexes(vertexes), uiElements(elements)
{
	// Load The Data
	glGenBuffersARB( 1, &uiVBO );					// Get A Valid Name
	glBindBufferARB( GL_ARRAY_BUFFER_ARB, uiVBO );			// Bind The Buffer
	glBufferDataARB( GL_ARRAY_BUFFER_ARB, vertexes * 3 * sizeof(float), data, GL_STATIC_DRAW_ARB );

	// Load The Indices
	glGenBuffersARB( 1, &uiIndexVBO );					// Get A Valid Name
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, uiIndexVBO);
	glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, elements * sizeof(unsigned int), indices, GL_STATIC_DRAW_ARB );
}

ElementVBO::~ElementVBO()
{
	glDeleteBuffersARB(1, &uiVBO);
	glDeleteBuffersARB(1, &uiIndexVBO);
}

void ElementVBO::Bind()
{
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, uiVBO );
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, uiIndexVBO);
	glVertexPointer(3 * uiVertexes, GL_FLOAT, 0, 0);
}

void ElementVBO::Unbind()
{
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
}


void ElementVBO::Render()
{
	glEnableClientState(GL_VERTEX_ARRAY);   

	Bind();

	glDrawElements(GL_TRIANGLES, uiElements, GL_UNSIGNED_INT, 0);

	Unbind();

	glDisableClientState( GL_VERTEX_ARRAY );
}


//glVertexAttribPointer?

VBOEntry::VBOEntry(FuncPointer pointer, GLint size, GLenum type, unsigned int offset)
	: fnPointer(pointer), iSize(size), eType(type), pOffset((void *)offset)
{

}

VBO::VBO(void *data, GLsizei stride, unsigned int count)
	: uiStride(stride), uiCount(count)
{
	glGenBuffersARB( 1, &uiVBO );					// Get A Valid Name
	glBindBufferARB( GL_ARRAY_BUFFER_ARB, uiVBO );			// Bind The Buffer
	glBufferDataARB( GL_ARRAY_BUFFER_ARB, stride * count, data, GL_STATIC_DRAW_ARB );

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

void VBO::AddEntry(FuncPointer pointer, GLint size, GLenum type, unsigned int offset)
{
	aEntry.push_back(new VBOEntry(pointer, size, type, offset));
}

void VBO::Bind()
{
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, uiVBO);
	for (unsigned int i = 0; i < aEntry.size(); i++)
	{
		(*aEntry[i]->fnPointer)(aEntry[i]->iSize, aEntry[i]->eType, uiStride, aEntry[i]->pOffset);
	}
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
}
void VBO::Bind(FuncPointer funcPointer, GLint size, GLenum type, unsigned int offset)
{
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, uiVBO);
	(*funcPointer)(size, type, uiStride, (void *)offset);
	// Unbind for safety
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
}

