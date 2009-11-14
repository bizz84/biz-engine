/*****************************************************************************
 * Filename			SDLShell.cpp
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

#include "SDLShell.h"
#include "SDL/SDL.h"
#include "assert.h"
#include "Misc.h"

#include "TTFont.h"

extern Shell *Run();
SDLShell *shell = NULL;

enum ExitStage
{
	EXIT_INIT_APP,
	EXIT_NO_SCREEN,
	EXIT_NO_FONT,
	EXIT_INIT_GL,
};

void Exit(ExitStage stage, SDLShell *shell)
{
	switch (stage)
	{
		case EXIT_INIT_GL:
			shell->ReleaseGL();
			// Don't break here since everything else needs to be released
		case EXIT_NO_SCREEN:
		case EXIT_NO_FONT:
		case EXIT_INIT_APP:
			shell->ReleaseApp();
			delete shell;
			SDL_Quit();
			exit(1);
			break;
		default:
			break;
	}

}

int main(int argc, char *argv[])
{
	shell = (SDLShell *)Run();

	int done;
	Uint8 *keys;

	if (SDL_Init(SDL_INIT_VIDEO))
	{
		printf("Unable to initialize SDL: %s\n", SDL_GetError());
		return 0;
	}

	shell->ProcessCommandLine(argc, argv);

	if (!shell->InitApp())
	{
		Exit(EXIT_INIT_APP, shell);
	}

	SDL_Surface *screen;

	if (Verbose(VerboseAll))
	{
		printf("Setting width = %d, height = %d, bpp = %d\n",
			shell->ShellGet(SDLShell::SHELL_WIDTH),
			shell->ShellGet(SDLShell::SHELL_HEIGHT),
			shell->ShellGet(SDLShell::SHELL_BPP));
	}
	screen = SDL_SetVideoMode(shell->ShellGet(SDLShell::SHELL_WIDTH),
				shell->ShellGet(SDLShell::SHELL_HEIGHT),
				shell->ShellGet(SDLShell::SHELL_BPP), shell->Flags());

	if (!screen)
	{
		fprintf(stderr, "Couldn't set %dx%d GL video mode: %s\n", 
			shell->ShellGet(SDLShell::SHELL_WIDTH),
				shell->ShellGet(SDLShell::SHELL_HEIGHT), SDL_GetError());
		
		Exit(EXIT_NO_SCREEN, shell);
	}
	SDL_WM_SetCaption(shell->GetTitle(), shell->GetTitle());

#ifdef SUPPORT_TTF
	if (TTF_Init() == -1) 
	{
		printf("Unable to initialize SDL_ttf: %s \n", TTF_GetError());
		Exit(EXIT_NO_FONT, shell);
	}
#endif
	shell->SetScreen(screen);

	shell->InitGL();

	done = 0;
	while (!done)
	{
		SDL_Event event;

		while ( SDL_PollEvent(&event) ) 
		{
			switch(event.type)
			{
			case SDL_VIDEORESIZE:
				if (shell->ShellGet(SDLShell::SHELL_RESIZABLE))
				{				
					screen = SDL_SetVideoMode(event.resize.w, event.resize.h,
								shell->ShellGet(SDLShell::SHELL_BPP),
								shell->Flags());
					if (screen)
					{
						shell->SetScreen(screen);
						shell->Reshape(screen->w, screen->h);
					}
					else
					{
						/* Uh oh, we couldn't set the new video mode?? */;
					}
				}
				break;
			case SDL_QUIT:
				done = 1;
				break;
			}
		}
		keys = SDL_GetKeyState(NULL);

		if (keys[SDLK_ESCAPE])
		{
			done = 1;
		}

		if (!shell->Render())
			break;

		SDL_GL_SwapBuffers();
	}
	shell->ReleaseGL();
	shell->ReleaseApp();

	SDL_Quit();
	delete shell;
	return 0;             /* ANSI C requires main to return int. */
}

SDLShell::SDLShell() : pScreen(NULL)
{
	ShellSet(SHELL_WIDTH, 640);
	ShellSet(SHELL_HEIGHT, 480);
	ShellSet(SHELL_BPP, 32);
	ShellSet(SHELL_FULLSCREEN, 0);	
	ShellSet(SHELL_RESIZABLE, 1);	
}

SDLShell::~SDLShell()
{


}

unsigned int SDLShell::Flags()
{
	unsigned int flags = SDL_OPENGL;//SDL_OPENGLBLIT;
	if (ShellGet(SHELL_FULLSCREEN)) flags |= SDL_FULLSCREEN;
	if (ShellGet(SHELL_RESIZABLE)) flags |= SDL_RESIZABLE;
	return flags;
}

bool SDLShell::Reshape(unsigned int width, unsigned int height)
{
	ShellSet(SHELL_WIDTH, width);
	ShellSet(SHELL_HEIGHT, height);

	return Resize(width, height);
}

unsigned int SDLShell::ShellGet(ShellParameter param)
{
	assert(param >= 0 && param < SHELL_NUM_PARAMETERS);
	return auiShellParams[param];
}

void SDLShell::ShellSet(ShellParameter param, unsigned int value)
{
	assert(param >= 0 && param < SHELL_NUM_PARAMETERS);
	auiShellParams[param] = value;
}

void SDLShell::ProcessCommandLine(int argc, char *argv[])
{
	if (argc == 0)
		return;

	strcpy(szTitle, argv[0] + 2);

	// Command line processing
	int i, value;
	for (i = 1; i < argc; i++)
	{
		char *pos = strchr(argv[i], '=');
		if (pos != NULL)
		{
			*pos = '\0';
			aCmdLineParams.push_back(CmdLineParameter(argv[i], pos + 1));
		}
		else
		{
			aCmdLineParams.push_back(CmdLineParameter(argv[i], (char *)""));
		}
	}
	for (i = 0; i < (int)aCmdLineParams.size(); i++)
	{
		if (Verbose(VerboseAll))
		{
			printf("CmdLineParam(%d) = [%s,%s]\n", i,
				aCmdLineParams[i].sName.c_str(), aCmdLineParams[i].sValue.c_str());
		}
		if (aCmdLineParams[i].sName.compare("width") == 0)
		{
			if ((value = atoi(aCmdLineParams[i].sValue.c_str())) > 0)
				ShellSet(SHELL_WIDTH, value);
		}
		else if (aCmdLineParams[i].sName.compare("height")  == 0)
		{
			if ((value = atoi(aCmdLineParams[i].sValue.c_str())) > 0)
				ShellSet(SHELL_HEIGHT, value);
		}
		else if (aCmdLineParams[i].sName.compare("fullpScreen") == 0)
		{
			if ((value = atoi(aCmdLineParams[i].sValue.c_str())) > 0)
				ShellSet(SHELL_FULLSCREEN, value);
		}
		else if (aCmdLineParams[i].sName.compare("resizable") == 0)
		{
			if ((value = atoi(aCmdLineParams[i].sValue.c_str())) > 0)
				ShellSet(SHELL_RESIZABLE, value);
		}
		else if (aCmdLineParams[i].sName.compare("verbose") == 0)
		{
			if ((value = atoi(aCmdLineParams[i].sValue.c_str())) >= 0)
				SetVerboseLevel((VerboseLevel)value);
		}
	}	
}

