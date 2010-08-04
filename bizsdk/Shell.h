/*****************************************************************************
 * Filename			Shell.h
 * 
 * License			GPLv3
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

	virtual bool InitApp() = 0;
	virtual bool InitGL() = 0;
	virtual bool Render() = 0;
	virtual bool ReleaseGL() = 0;
	virtual bool ReleaseApp() = 0;
	virtual bool Resize(unsigned int width, unsigned int height) = 0;

	virtual bool RequiresOpenGL2() = 0;
	virtual bool RequiresTTF() = 0;

	virtual const char *GetAppName() const = 0;
	virtual const char *GetAppVersion() const = 0;
public:
	Shell() { }
	virtual ~Shell() { }

	virtual int Run(int argc, char *argv[]) = 0;
};

#endif
