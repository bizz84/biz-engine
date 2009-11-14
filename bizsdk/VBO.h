/*****************************************************************************
 * Filename			VBO.h
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

#ifndef _VBO_H_
#define _VBO_H_

#include <GL/glu.h>
#include <vector>

using namespace std;

class ElementVBO
{
	GLuint uiVBO;
	GLuint uiIndexVBO;

	unsigned int uiVertexes;
	unsigned int uiElements;
public:
	ElementVBO(float *data, unsigned int *indices, unsigned int vertexes, unsigned int elements);
	~ElementVBO();

	void Bind();
	void Unbind();
	void Render();
};


typedef void (*FuncPointer)(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);

class VBOEntry
{
public:
	FuncPointer fnPointer;
	GLint iSize;
	GLenum eType;
	void *pOffset;

	VBOEntry(FuncPointer pointer, GLint size, GLenum type, unsigned int offset);
};

class VBO
{
	vector<VBOEntry *> aEntry;

	GLuint uiVBO;
	GLsizei uiStride;
	unsigned int uiCount;
public:
	VBO(void *data, GLsizei stride, unsigned int count);
	~VBO();

	void AddEntry(FuncPointer pointer, GLint size, GLenum type, unsigned int offset);

	void Bind();
	void Bind(FuncPointer funcPointer, GLint size, GLenum type, unsigned int offset);
};

#endif

