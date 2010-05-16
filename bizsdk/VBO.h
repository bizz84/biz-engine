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

#include "Extensions.h"
#include <vector>

using namespace std;

typedef void (*ArrayFuncPointer)(GLint size, GLenum type, GLsizei stride,
	const GLvoid *pointer);

enum {
	VERTEX_POINTER_FLAG   = 0x01U,
	NORMAL_POINTER_FLAG   = 0x02U,
	COLOR_POINTER_FLAG    = 0x04U,
	TEXCOORD_POINTER_FLAG = 0x08U,
};

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
	unsigned short usFlags;

	void *pVertexOffset, *pNormalOffset, *pColorOffset, *pTexCoordOffset;
	unsigned int uiVertexSize, uiColorSize, uiTexCoordSize;


public:
	VBO(void *data, GLsizei stride, unsigned int count);
	virtual ~VBO();

	void AddEntry(ArrayFuncPointer pointer, GLint size, GLenum type,
		unsigned int offset);

	virtual void Bind();
	virtual void Bind(ArrayFuncPointer funcPointer, GLint size, GLenum type,
		unsigned int offset);

	virtual void Render(GLenum mode);

	const unsigned int GetCount() const { return uiCount; }

	void SetVertexData(unsigned int offset = 0, unsigned int size = 3);
	void SetNormalData(unsigned int offset);
	void SetColorData(unsigned int offset, unsigned int size = 3);
	void SetTexCoordData(unsigned int offset, unsigned int size = 2);

	const GLuint GetVBO() const { return uiVBO; }
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

	virtual void Render(GLenum mode);

	const unsigned int GetElements() const { return uiElements; }

	const GLuint GetIndexedVBO() const { return uiIndexVBO; }

};

#endif

