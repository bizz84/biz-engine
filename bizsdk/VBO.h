/*****************************************************************************
 * Filename			VBO.h
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

#ifndef _VBO_H_
#define _VBO_H_

#include "Extensions.h"

#include "boost/ptr_container/ptr_list.hpp"
using namespace boost;

//! Type defining the a function pointer used to specify a vertex array.
//! Functions like glVertexPointer have such signature
typedef void (*ArrayFuncPointer)(GLint size, GLenum type, GLsizei stride,
	const GLvoid *pointer);

//! Enumeration listing the flags for the vertex arrays to be bound when
//! rendering
enum {
	VERTEX_POINTER_FLAG   = 0x01U, /*!< calls glVertexPointer. */  
	NORMAL_POINTER_FLAG   = 0x02U, /*!< calls glNormalPointer. */  
	COLOR_POINTER_FLAG    = 0x04U, /*!< calls glColorPointer. */  
	TEXCOORD_POINTER_FLAG = 0x08U, /*!< calls glTexCoordPointer. */  
};

//! Class defining a Vertex Buffer Object
/*!
 VBO is a RAII class that acts as a wrapper for the GL commands used to store and
 render a vertex buffer object.
 Support is included for the common GL commons that allow to specify specific
 vertex arrays (see enumeration), in addition to a list of function pointers
 that can be additionally bound and used as attributes in the vertex shader.
 Only generic vertex pointers should be specified in recent versions of the GL
 spec, although this class provides backwards compatibility with the old
 "glVertexPointer" type calls.
 */
class VBO
{
protected:
	//! Functor class defining a generic vertex array that can be bound at
	//! render time
	class VBOEntry
	{
	public:
		//! Actual function to be called
		// TODO: Get rid of this and always call glVertexAttribPointer?
		ArrayFuncPointer fnPointer;
		//! size
		GLint iSize;
		//! type
		GLenum eType;
		//! Offset into the array
		/*!
		 This would normally be a pointer to the location where the vertex
		 data is stored, but since we are rendering a VBO, it is an offset.
		 */
		void *pOffset;

		//! Constructor
		VBOEntry(ArrayFuncPointer pointer, GLint size, GLenum type,
			unsigned int offset)
			: fnPointer(pointer), iSize(size), eType(type),
			pOffset((void *)offset) { }

		//! Call operator
		void operator()(GLsizei stride) const
		{
			(*fnPointer)(iSize, eType, stride, pOffset);
		}
	};
	//! List of entries to be called
	ptr_list<VBOEntry> aEntry;
	
	//! Handle returned by GL when creating the VBO
	GLuint uiVBO;
	//! Stride
	GLsizei uiStride;
	//! Number of elements (this is multiplied by the stride when creating the
	//! VBO with glBufferData()
	unsigned int uiCount;
	//! Flags used to determine which attribute arrays need to be bound
	/*!
	 Each of the Set*Data() functions adds the corresponding enumeration value
	 as a mask to this variable
	 */
	unsigned short usFlags;

	//! Offsets to the standard vertex arrays
	void *pVertexOffset, *pNormalOffset, *pColorOffset, *pTexCoordOffset;
	//! Sizes of the standard vertex array elements (normally 2, 3 or 4)
	unsigned int uiVertexSize, uiColorSize, uiTexCoordSize;


public:
	//! Constructor
	VBO(void *data, GLsizei stride, unsigned int count);
	virtual ~VBO();

	//! Adds an additional vertex array
	void AddEntry(ArrayFuncPointer pointer, GLint size, GLenum type,
		unsigned int offset);
		
	//! Binds the VBO
	/*!
	 Binds the VBO and all the corresponding arrays (standard ones + all the
	 entries originally added with AddEntry()
	 */
	virtual void Bind() const;
	//! Binds an array passed at runtime
	virtual void Bind(ArrayFuncPointer funcPointer, GLint size, GLenum type,
		unsigned int offset) const;

	//! Binds the array automatically and calls glDrawArrays
	/*!
	 Use this call when the VBO is rendered only once in the code
	 */
	virtual void Render(GLenum mode) const;
	//! Calls glDrawArrays
	/*!
	 Use this call when the VBO is rendered multiple times and it is not
	 necessary to bind the vertex arrays each time (explicitly bind before
	 the first call.
	 */
	virtual void Draw(GLenum mode) const;

	//! Sets the vertex data values used when binding
	void SetVertexData(unsigned int offset = 0, unsigned int size = 3);
	//! Sets the normal data values used when binding
	/*!
	 Note that the size parameter is not specified since it is assumed
	 to be 3 by glNormalPointer()
	 */
	void SetNormalData(unsigned int offset);
	//! Sets the color data values used when binding
	void SetColorData(unsigned int offset, unsigned int size = 3);
	//! Sets the texcoord data values used when binding
	void SetTexCoordData(unsigned int offset, unsigned int size = 2);

	//! Getter for the number of elements
	const unsigned int GetCount() const { return uiCount; }

	//! Returns handle to VBO
	const GLuint GetVBO() const { return uiVBO; }
};

//! Class defining an indexed Vertex Buffer Object
/*!
 IndexedVBO subclasses from VBO by adding index information, in order to render
 with glDrawElements().
 */
class IndexedVBO : public VBO
{
protected:
	//! Handle returned by GL when creating the index VBO
	GLuint uiIndexVBO;
	//! Number of elements in the index array
	unsigned int uiElements;
public:
	//! Constructor
	IndexedVBO(void *data, GLsizei stride, unsigned int count, void *indices,
		unsigned int elements);

	//! Binds element array buffer. Internally calls VBO::Bind()
	virtual void Bind() const;
	//! Unbinds the element array buffer
	virtual void Unbind() const;

	//! Binds the array automatically and calls glDrawElements
	/*!
	 Use this call when the VBO is rendered only once in the code
	 */
	virtual void Render(GLenum mode) const;
	//! Calls glDrawElements
	/*!
	 Use this call when the VBO is rendered multiple times and it is not
	 necessary to bind the vertex arrays each time (explicitly bind before
	 the first call.
	 */
	virtual void Draw(GLenum mode) const;

	//! Getter for the number of elements
	const unsigned int GetElements() const { return uiElements; }

	//! Returns handle to VBO index buffer
	const GLuint GetIndexedVBO() const { return uiIndexVBO; }
};

#endif

