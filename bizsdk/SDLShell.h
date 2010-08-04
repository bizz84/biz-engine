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
public:
	enum ShellParameter { SHELL_WIDTH = 0, SHELL_HEIGHT, SHELL_BPP,
		SHELL_FULLSCREEN, SHELL_RESIZABLE, SHELL_VSYNC, SHELL_NUM_PARAMETERS };
private:
	int shellFrame;

	auto_ptr<Pointer> pPointer;
	bool pressed[128];
	bool pressing[128];
	bool scrollUp;
	bool scrollDown;
	bool leftClick;
	bool rightClick;
	
	vector<CmdLineParameter> aCmdLineParams;
	unsigned int auiShellParams[SHELL_NUM_PARAMETERS];

	void SetPressed(Uint8 key, bool value) { pressed[key] = value; }
	void SetPressing(Uint8 key, bool value) { pressing[key] = value; }
	void ResetPressed();
	void ResetMouse();	
protected:

	void Exit(ExitStage stage);

	void ProcessCommandLine(int argc, char *argv[]);

	vector<CmdLineParameter> &GetCommandLineParams() { return aCmdLineParams; }

	bool Reshape(unsigned int width, unsigned int height);

	unsigned int Flags();

	void CreatePointer();
	virtual Pointer *NewPointer() { return new Pointer(this); }
	
public:
	SDLShell();
	virtual ~SDLShell() { }

	virtual int Run(int argc, char *argv[]);

	const unsigned int ShellGet(ShellParameter param) const;
	void ShellSet(ShellParameter param, unsigned int value);

	// Getters
	const Pointer *GetPointer() const { return pPointer.get(); }
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

