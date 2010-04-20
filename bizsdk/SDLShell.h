/*****************************************************************************
 * Filename			SDLShell.h
 * 
 * License			LGPL
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

#include <vector>
#include <string>

using namespace std;

class CmdLineParameter
{
public:
	CmdLineParameter(char *pszName, char *pszValue)
		: sName(pszName), sValue(pszValue) { }
	string sName;
	string sValue;
};

enum ExitStage
{
	EXIT_INIT_APP,
	EXIT_NO_SCREEN,
	EXIT_NO_FONT,
	EXIT_INIT_GL,
};

class SDLShell : public Shell
{
protected:
	Pointer *pPointer;
	bool pressed[128];
	bool pressing[128];
	bool scrollUp;
	bool scrollDown;

	void Exit(ExitStage stage);

	void ProcessCommandLine(int argc, char *argv[]);

	vector<CmdLineParameter> &GetCommandLineParams() { return aCmdLineParams; }

	bool Reshape(unsigned int width, unsigned int height);

	unsigned int Flags();

	void CreatePointer();

	bool KeyPressed(Uint8 key) { return pressed[key]; }
	bool KeyPressing(Uint8 key) { return pressing[key]; }

	bool ScrollUp() { return scrollUp; }
	bool ScrollDown() { return scrollDown; }
	
public:
	SDLShell();
	virtual ~SDLShell();

	virtual int Run(int argc, char *argv[]);

	enum ShellParameter { SHELL_WIDTH = 0, SHELL_HEIGHT, SHELL_BPP,
		SHELL_FULLSCREEN, SHELL_RESIZABLE, SHELL_VSYNC, SHELL_NUM_PARAMETERS };

	unsigned int ShellGet(ShellParameter param);	
	void ShellSet(ShellParameter param, unsigned int value);

	Pointer *GetPointer() { return pPointer; }
private:
	vector<CmdLineParameter> aCmdLineParams;
	unsigned int auiShellParams[SHELL_NUM_PARAMETERS];

	void SetPressed(Uint8 key, bool value) { pressed[key] = value; }
	void SetPressing(Uint8 key, bool value) { pressing[key] = value; }
	void ResetPressed();
	void ResetScroll();
};

#endif

