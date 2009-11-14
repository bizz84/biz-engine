/*****************************************************************************
 * Filename			Shell.h
 * 
 * License			LGPL
 *
 * Author			Andrea Bizzotto (bizz84@gmail.com)
 *
 * Platform			LinuxX11 / OpenGL
 * 
 * Description		Shell abstraction
 *
 *****************************************************************************/

#ifndef _SHELL_H_
#define _SHELL_H_

class Shell
{
protected:

public:
	Shell() { }
	virtual ~Shell() { }

	virtual bool InitApp() = 0;
	virtual bool InitGL() = 0;
	virtual bool Render() = 0;
	virtual bool ReleaseGL() = 0;
	virtual bool ReleaseApp() = 0;
	virtual bool Resize(unsigned int width, unsigned int height) = 0;
};

#endif
