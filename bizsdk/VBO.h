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

typedef void (*ArrayFuncPointer)(GLint size, GLenum type, GLsizei stride,
	const GLvoid *pointer);

class VBO
{
protected:
	class VBOEntry
	{
	public:
		ArrayFuncPointer fnPointer;
		GLint iSize;
		GLenum eType;
		void *pOffset;

		VBOEntry(ArrayFuncPointer pointer, GLint size, GLenum type,
			unsigned int offset)
			: fnPointer(pointer), iSize(size), eType(type),
			pOffset((void *)offset) { }
	};
	vector<VBOEntry *> aEntry;

	GLuint uiVBO;
	GLsizei uiStride;
	unsigned int uiCount;
public:
	VBO(void *data, GLsizei stride, unsigned int count);
	~VBO();

	void AddEntry(ArrayFuncPointer pointer, GLint size, GLenum type,
		unsigned int offset);

	virtual void Bind();
	virtual void Bind(ArrayFuncPointer funcPointer, GLint size, GLenum type,
		unsigned int offset);

	virtual void Render();

	unsigned int GetCount() { return uiCount; }
};

class IndexedVBO : public VBO
{
protected:
	GLuint uiIndexVBO;
	unsigned int uiElements;
public:
	IndexedVBO(void *data, GLsizei stride, unsigned int count, void *indices,
		unsigned int elements);

	virtual void Bind();
	virtual void Unbind();

	virtual void Render();

	unsigned int GetElements() { return uiElements; }
};

#endif

