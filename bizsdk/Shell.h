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

//! Abstract class for defining all application entry points (template method)
class Shell
{
protected:
	//! Initialization. Called before the GL window is created
	virtual bool InitApp() = 0;
	//! GL related initialization. Called after the GL window is created
	virtual bool InitGL() = 0;
	//! Render code. Called within the main loop
	virtual bool Render() = 0;
	//! Called when the main loop exits. Should release all GL objects
	virtual bool ReleaseGL() = 0;
	//! Called after the GL window has been destroyed.
	virtual bool ReleaseApp() = 0;
	//! Called when a window resize event is launched
	virtual bool Resize(unsigned int width, unsigned int height) = 0;

	//! Whether the application requires OpenGL 2 support
	/*!	
	 This should cause the application to terminate if OpenGL 2 support is
	 required but not supported
	 */
	virtual bool RequiresOpenGL2() = 0;
	//! Whether the application will make use of TTF fonts
	/*!	
	 This should cause the application to terminate if TTF font support is
	 required but not supported
	 */
	virtual bool RequiresTTF() = 0;

	//! Application name displayed on the window bar
	virtual const char *GetAppName() const = 0;
	//! Application version displayed on the window bar
	virtual const char *GetAppVersion() const = 0;
public:
	//! Constructor
	Shell() { }
	//! Destructor
	virtual ~Shell() { }

	//! Wrapper for the application entry point
	virtual int Run(int argc, char *argv[]) = 0;
};

#endif
