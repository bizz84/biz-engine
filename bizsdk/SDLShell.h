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
#include "GL/glu.h"
#include "SDL/SDL.h"

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

class SDLShell : public Shell
{
	SDL_MouseMotionEvent sMouseMotion;
	SDL_MouseButtonEvent sMouseButton;

protected:
	Pointer *pPointer;
	
public:
	SDLShell();
	virtual ~SDLShell();

	enum ShellParameter { SHELL_WIDTH = 0, SHELL_HEIGHT, SHELL_BPP,
		SHELL_FULLSCREEN, SHELL_RESIZABLE, SHELL_NUM_PARAMETERS };

	unsigned int ShellGet(ShellParameter param);	
	void ShellSet(ShellParameter param, unsigned int value);

	char *GetTitle() { return szTitle; }

	void ProcessCommandLine(int argc, char *argv[]);

	vector<CmdLineParameter> &GetCommandLineParams() { return aCmdLineParams; }

	bool Reshape(unsigned int width, unsigned int height);

	unsigned int Flags();

	void CreatePointer();

	/* Mouse related functions */
	void UpdateMouseMotion(SDL_MouseMotionEvent &event);
	void UpdateMouseButton(SDL_MouseButtonEvent &event);
	void ResetMouse();
	void InitMouse();

	SDL_MouseMotionEvent &GetMouseMotion() { return sMouseMotion; }
	SDL_MouseButtonEvent &GetMouseButton() { return sMouseButton; }

	Pointer *GetPointer() { return pPointer; }
private:
	vector<CmdLineParameter> aCmdLineParams;
	char szTitle[50];
	unsigned int auiShellParams[SHELL_NUM_PARAMETERS];
};

#endif

