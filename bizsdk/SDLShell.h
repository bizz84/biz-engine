/*****************************************************************************
 * Filename			SDLShell.h
 * 
 * License			GPLv3
 *
 * Author			Andrea Bizzotto (bizz84@gmail.com)
 *
 * Platform			LinuxX11 / OpenGL
 * 
 * Description		SDL Shell
 *
 *****************************************************************************/

#ifndef _SDL_SHELL_H_
#define _SDL_SHELL_H_

#include "Shell.h"
#ifdef __linux__
#include <SDL/SDL.h>
#else
#include <SDL.h>
#endif

#include "Pointer.h"

#include <memory>
#include <vector>
#include <string>

using namespace std;

//! Wrapper class to store a command line parameter of the form "name=value"
class CmdLineParameter
{
public:
	//! Constructor
	CmdLineParameter(char *pszName, char *pszValue)
		: sName(pszName), sValue(pszValue) { }
	//! name of the parameter
	string sName;
	//! value of the parameter (literal, can be converted)
	string sValue;
};

//! Conditions for early failure
enum ExitStage
{
	EXIT_INIT_APP,       /*!< User controlled quit from InitApp() */
	EXIT_NO_GL2_SUPPORT, /*!< No OpenGL 2 support */
	EXIT_NO_SCREEN,      /*!< SDL could not set the video mode */
	EXIT_NO_FONT,        /*!< True type font library couldn't be initialized */
	EXIT_INIT_GL,        /*!< User controlled quit from InitGL() */
};

//! Class providing window, input and main loop control.
/*!
 SDLShell is a class providing window, input and main loop control. The main
 application needs to derive from SDLShell and override the virtuals to access
 the main loop.
 SDLShell provides a mechanism to read and parse command line parameters, which
 can be accessed at any time by the application.
 */
class SDLShell : public Shell
{
public:
	//! Enumeration defining the properties of the shell
	enum ShellParameter {
		SHELL_WIDTH = 0,     /*! Width */
		SHELL_HEIGHT,        /*! Height */
		SHELL_BPP,           /*! Bits per pixel. Set in SDL_SetVideoMode() */
		SHELL_FULLSCREEN,    /*! Fullscreen */
		SHELL_RESIZABLE,     /*! Whether the window is resizable */
		SHELL_VSYNC,         /*! Vsync enabled/disabled */
		SHELL_NUM_PARAMETERS /*! Number of parameters */
	};
private:
	//! Frame counter
	int shellFrame;

	//! Instance of pointer device (mouse, normally)
	auto_ptr<Pointer> pPointer;
	//! key pressed
	/*!
	 Elements in this array are set to true when the corresponding key is hit,
	 but are reset to false each frame and do not hold the value even if the
	 key is still down.
	 */
	bool pressed[128];
	//! key pressing
	/*!
	 Elements in this array have a one-to-one correspondence with the up/down
	 state of the key
	 */
	bool pressing[128];
	//! true when a mouse scroll up event is read
	bool scrollUp;
	//! true when a mouse scroll down event is read
	bool scrollDown;
	//! true when a mouse left click event is read
	bool leftClick;
	//! true when a mouse right click event is read
	bool rightClick;
	
	//! Array storing the name-value pairs from the command line
	vector<CmdLineParameter> aCmdLineParams;
	//! Shell parameters (see ShellParameter enumeration)
	unsigned int auiShellParams[SHELL_NUM_PARAMETERS];

	//! Setter for the "pressed" array
	void SetPressed(Uint8 key, bool value) { pressed[key] = value; }
	//! Setter for the "pressing" array
	void SetPressing(Uint8 key, bool value) { pressing[key] = value; }
	//! Resets all elements in "pressed" to false
	void ResetPressed();
	//! Resets all mouse related variables to false
	void ResetMouse();	

	//! Flags to be generated from the shell parameters configuration and 
	//! passed to SDL_SetVideoMode()
	unsigned int Flags();

	//! User-defined window reshape function
	/*!
	 This function is called when the user reshapes the rendering window.
	 This causes the viewport to be updated to the new width and height, the
	 relative SHELL_WIDTH and SHELL_HEIGHT to be updated, and the virtual
	 method Resize() to be called to perform user-defined event handling.
	 */
	bool Reshape(unsigned int width, unsigned int height);

	//! Exits program after unloading appropriate resources
	void Exit(ExitStage stage);

	//! Reads command line into the name-value array
	/*!
	 All name-value pairs are saved in local storage for usage by the
	 application. Entries that match the names for the default shell parameters
	 are applied automatically. This allows for example to change the width
	 and height of the window via command line (since these value are used to
	 create the window after this method is called)
	 */
	void ProcessCommandLine(int argc, char *argv[]);

	//! Creates the pointer instance (uses NewPointer())
	void CreatePointer();
protected:

	//! Getter for array of command line parameters
	inline const vector<CmdLineParameter> &GetCommandLineParams() const
		{ return aCmdLineParams; }

	//! Creates a new pointer object (can be user-defined)
	virtual auto_ptr<Pointer> NewPointer()
	{
		return auto_ptr<Pointer>(new Pointer(this));
	}
	
public:
	//! Constructor
	SDLShell();
	//! Destructor
	virtual ~SDLShell() { }

	//! Application main-loop
	/*!
	 This is the entry point of the application, which is directly called by
	 main() after creating the Shell instance.
	 Run performs in order:
	 - Call to SDL_Init()
	 - ProcessCommandLine()
	 - InitApp()
	 - Create rendering surface and set video mode
	 - Check if required GL version is supported
	 - Create pointer
	 - InitGL()
	 - Main loop
		-# Read mouse input
		-# Poll all relevant events and update state accordingly
		-# Check for quit condition (escape key)
		-# Call Render()
		-# swap buffers
		-# increment frame count
	 - ReleaseGL()
	 - ReleaseApp()
	 - SDL_Quit()
	 */
	virtual int Run(int argc, char *argv[]);

	//! Getter for specific parameter
	const unsigned int ShellGet(ShellParameter param) const;
	//! Setter for specific parameter
	void ShellSet(ShellParameter param, unsigned int value);

	//! Get pointer
	const Pointer *GetPointer() const { return pPointer.get(); }
	//! Get pointer
	Pointer *GetPointer() { return pPointer.get(); }

	const bool KeyPressed(Uint8 key) const { return pressed[key]; }
	const bool KeyPressing(Uint8 key) const { return pressing[key]; }

	const bool ScrollUp() const { return scrollUp; }
	const bool ScrollDown() const { return scrollDown; }

	const bool LeftClick() const { return leftClick; }
	const bool RightClick() const { return rightClick; }

	const int GetCurrentFrame() const { return shellFrame; }
};

#endif

