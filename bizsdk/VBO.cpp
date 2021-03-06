/*****************************************************************************
 * Filename			VBO.cpp
 * 
 * License			GPLv3
 *
 * Author			Andrea Bizzotto (bizz84@gmail.com)
 *
 * Platform			LinuxX11 / OpenGL
 * 
 * Description		OpenGL Vertex Buffer Object
 *
 *****************************************************************************/

#include "VBO.h"

/*****************************************************************************
 * VBO class implementation
 *****************************************************************************/
VBO::VBO(void *data, GLsizei stride, unsigned int count)
	: uiStride(stride), uiCount(count), usFlags(VERTEX_POINTER_FLAG),
	pVertexOffset(NULL), pNormalOffset(NULL), pColorOffset(NULL),
	pTexCoordOffset(NULL), uiVertexSize(3), uiColorSize(3), uiTexCoordSize(2)
{
	glGenBuffers(1, &uiVBO);					
	glBindBuffer(GL_ARRAY_BUFFER_ARB, uiVBO);
	glBufferData(GL_ARRAY_BUFFER_ARB, stride * count, data,
		GL_STATIC_DRAW_ARB);

	// Unbind for safety
	glBindBuffer(GL_ARRAY_BUFFER_ARB, 0);
}

VBO::~VBO()
{
	glDeleteBuffers(1, &uiVBO);
}

void VBO::SetVertexData(unsigned int offset, unsigned int size)
{
	usFlags |= VERTEX_POINTER_FLAG;
	pVertexOffset = (void *)offset;
	uiVertexSize = size;
}

void VBO::SetNormalData(unsigned int offset)
{
	usFlags |= NORMAL_POINTER_FLAG;
	pNormalOffset = (void *)offset;
}

void VBO::SetColorData(unsigned int offset, unsigned int size)
{
	usFlags |= COLOR_POINTER_FLAG;
	pColorOffset = (void *)offset;
	uiColorSize = size;
}

void VBO::SetTexCoordData(unsigned int offset, unsigned int size)
{
	usFlags |= TEXCOORD_POINTER_FLAG;
	pTexCoordOffset = (void *)offset;
	uiTexCoordSize = size;
}


void VBO::AddEntry(ArrayFuncPointer pointer, GLint size, GLenum type,
	unsigned int offset)
{
	aEntry.push_back(new VBOEntry(pointer, size, type, offset));
}


void VBO::Bind() const
{
	glBindBuffer(GL_ARRAY_BUFFER_ARB, uiVBO);
	ptr_list<VBOEntry>::const_iterator iter;
	for (iter = aEntry.begin(); iter != aEntry.end(); iter++)
	{
		// Calls functor
		(*iter)(uiStride);
	}
	if (usFlags & VERTEX_POINTER_FLAG)
	{
		glVertexPointer(uiVertexSize, GL_FLOAT, uiStride, pVertexOffset);
	}
	if (usFlags & NORMAL_POINTER_FLAG)
	{
		glNormalPointer(GL_FLOAT, uiStride, pNormalOffset);
	}
	if (usFlags & COLOR_POINTER_FLAG)
	{
		glColorPointer(uiColorSize, GL_FLOAT, uiStride, pColorOffset);
	}
	if (usFlags & TEXCOORD_POINTER_FLAG)
	{
		glTexCoordPointer(uiTexCoordSize, GL_FLOAT, uiStride, pTexCoordOffset);
	}
	glBindBuffer(GL_ARRAY_BUFFER_ARB, 0);
}

void VBO::Bind(ArrayFuncPointer funcPointer, GLint size, GLenum type,
	unsigned int offset) const
{
	glBindBuffer(GL_ARRAY_BUFFER_ARB, uiVBO);
	(*funcPointer)(size, type, uiStride, (void *)offset);
	glBindBuffer(GL_ARRAY_BUFFER_ARB, 0);
}

void VBO::Render(GLenum mode) const
{
	Bind();
	Draw(mode);
}
void VBO::Draw(GLenum mode) const
{
	glDrawArrays(mode, 0, GetCount());
}

/*****************************************************************************
 * IndexedVBO class implementation
 *****************************************************************************/
IndexedVBO::IndexedVBO(void *data, GLsizei stride, unsigned int count,
	void *indices, unsigned int elements)
	: VBO(data, stride, count), uiElements(elements)
{
	// Load The Indices
	glGenBuffers( 1, &uiIndexVBO );
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER_ARB, uiIndexVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER_ARB,
		elements * sizeof(GL_UNSIGNED_INT), indices, GL_STATIC_DRAW_ARB );	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
}

void IndexedVBO::Bind() const
{
	VBO::Bind();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER_ARB, uiIndexVBO);
}	

void IndexedVBO::Unbind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
}

void IndexedVBO::Render(GLenum mode) const
{
	Bind();
	Draw(mode);
	Unbind();
}

void IndexedVBO::Draw(GLenum mode) const
{
	// TODO: Use a member variable to switch between GL_UNSIGNED_INT and
	// GL_UNSIGNED_SHORT
	glDrawElements(mode, GetElements(), GL_UNSIGNED_INT, 0);
}
