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
#include "assert.h"
#include "Misc.h"
#include "Keys.h"

#include "TTFont.h"

#include "Extensions.h"

// Must be defined by the application
extern Shell *NewDemo();
Shell *shell = NULL;

SDLShell::SDLShell() : pPointer(NULL)
{
	shellFrame = 0;
	ShellSet(SHELL_WIDTH, 640);
	ShellSet(SHELL_HEIGHT, 480);
	ShellSet(SHELL_BPP, 32);
	ShellSet(SHELL_FULLSCREEN, 0);	
	ShellSet(SHELL_RESIZABLE, 1);
	ShellSet(SHELL_VSYNC, 0);

	for (unsigned int i = 0; i < 128; i++)
	{
		pressed[i] = false;
		pressing[i] = false;
	}
	scrollUp = false;
	scrollDown = false;
}

SDLShell::~SDLShell()
{
	delete pPointer;
}

void SDLShell::ResetPressed()
{
	for (unsigned int i = 0; i < 128; i++)
	{
		pressed[i] = false;
	}
}
void SDLShell::ResetScroll()
{
	scrollUp = false;
	scrollDown = false;
}

void SDLShell::Exit(ExitStage stage)
{
	switch (stage)
	{
		case EXIT_INIT_GL:
			ReleaseGL();
			// Don't break here since everything else needs to be released
		case EXIT_NO_SCREEN:
		case EXIT_NO_FONT:
		case EXIT_INIT_APP:
			ReleaseApp();
			delete shell;
			SDL_Quit();
			exit(1);
			break;
		default:
			break;
	}

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
	glViewport(0, 0, width, height);
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

void SDLShell::CreatePointer()
{
	if (pPointer == NULL)
	{
		pPointer = NewPointer();
	}
}

void SDLShell::ProcessCommandLine(int argc, char *argv[])
{
	if (argc == 0)
		return;

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
				aCmdLineParams[i].sName.c_str(),
				aCmdLineParams[i].sValue.c_str());
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
		else if (aCmdLineParams[i].sName.compare("fullscreen") == 0)
		{
			if ((value = atoi(aCmdLineParams[i].sValue.c_str())) > 0)
				ShellSet(SHELL_FULLSCREEN, value);
		}
		else if (aCmdLineParams[i].sName.compare("resizable") == 0)
		{
			if ((value = atoi(aCmdLineParams[i].sValue.c_str())) > 0)
				ShellSet(SHELL_RESIZABLE, value);
		}
		else if (aCmdLineParams[i].sName.compare("vsync") == 0)
		{
			if ((value = atoi(aCmdLineParams[i].sValue.c_str())) > 0)
				ShellSet(SHELL_VSYNC, value);
		}
		else if (aCmdLineParams[i].sName.compare("verbose") == 0)
		{
			if ((value = atoi(aCmdLineParams[i].sValue.c_str())) >= 0)
				SetVerboseLevel((VerboseLevel)value);
		}
	}	
}


int SDLShell::Run(int argc, char *argv[])
{
	printf("*** %s, V%s ***\n", GetAppName(), GetAppVersion());

	int done;
	Uint8 *keys;

	if (SDL_Init(SDL_INIT_VIDEO))
	{
		printf("Unable to initialize SDL: %s\n", SDL_GetError());
		return 0;
	}

	ProcessCommandLine(argc, argv);

	if (!InitApp())
	{
		Exit(EXIT_INIT_APP);
	}

	SDL_Surface *screen;

	if (Verbose(VerboseAll))
	{
		printf("Setting width = %d, height = %d, bpp = %d\n",
			ShellGet(SDLShell::SHELL_WIDTH),
			ShellGet(SDLShell::SHELL_HEIGHT),
			ShellGet(SDLShell::SHELL_BPP));
	}
	screen = SDL_SetVideoMode(ShellGet(SDLShell::SHELL_WIDTH),
				ShellGet(SDLShell::SHELL_HEIGHT),
				ShellGet(SDLShell::SHELL_BPP), Flags());

	if (!screen)
	{
		fprintf(stderr, "Couldn't set %dx%d GL video mode: %s\n", 
			ShellGet(SDLShell::SHELL_WIDTH),
				ShellGet(SDLShell::SHELL_HEIGHT), SDL_GetError());
		
		Exit(EXIT_NO_SCREEN);
	}
	char tempString[100];
	sprintf(tempString, "%s V%s", GetAppName(), GetAppVersion());
	SDL_WM_SetCaption(tempString, GetAppName());

	SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, ShellGet(SHELL_VSYNC));

	if (RequiresOpenGL2())
	{
		// Initialize GLEW and make sure OpenGL 2.0 is supported
		GLenum err = glewInit();
		if(GLEW_OK != err)
		{
		    fprintf(stderr, "[ Fail ] - Error: %s\n", glewGetErrorString(err));
		    return 0;
		}

		if(!GLEW_VERSION_2_0 &&
		   !(GLEW_ARB_shading_language_100 && GLEW_ARB_shader_objects &&
		     GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader))
		{
		    fprintf(stderr, "[ Fail ] - Shaders not supported\n");
			return 0;
		}

		if(!GLEW_ARB_vertex_buffer_object)
		{
		    fprintf(stderr, "[ Fail ] - VBO objects are not supported\n");
		    return 0;
		}
	}

	if (RequiresTTF())
	{

		if (TTF_Init() == -1) 
		{
			printf("Unable to initialize SDL_ttf: %s \n", TTF_GetError());
			Exit(EXIT_NO_FONT);
		}
	}

	CreatePointer();
	if (!InitGL())
	{
		Exit(EXIT_INIT_GL);
	}

	done = 0;
	while (!done)
	{
		pPointer->Input();
		ResetPressed();
		ResetScroll();

		SDL_Event event;

		while ( SDL_PollEvent(&event) ) 
		{
			switch(event.type)
			{
			case SDL_VIDEORESIZE:
				if (ShellGet(SDLShell::SHELL_RESIZABLE))
				{				
					screen = SDL_SetVideoMode(event.resize.w, event.resize.h,
								ShellGet(SDLShell::SHELL_BPP),
								Flags());
					if (screen)
					{
						Reshape(screen->w, screen->h);
					}
					else
					{
						/* Uh oh, we couldn't set the new video mode?? */;
					}
				}
				break;
            case SDL_MOUSEMOTION:
				pPointer->UpdateMouseMotion(event.motion);
				/*if (Verbose(VerboseAll))
				{
                	printf("Mouse moved by %d,%d to (%d,%d)\n", 
                       event.motion.xrel, event.motion.yrel,
                       event.motion.x, event.motion.y);
				}*/
                break;
            case SDL_MOUSEBUTTONDOWN:
				if (event.button.button == SDL_BUTTON_WHEELDOWN)
				{
					scrollDown = true;
				}
				pPointer->UpdateMouseButton(event.button);
				/*if (Verbose(VerboseAll))
				{
                	printf("Mouse button %d pressed at (%d,%d)\n",
                       event.button.button, event.button.x, event.button.y);
				}*/
                break;
            case SDL_MOUSEBUTTONUP:
				if (event.button.button == SDL_BUTTON_WHEELUP)
				{
					scrollUp = true;
				}
				pPointer->UpdateMouseButton(event.button);
				/*if (Verbose(VerboseAll))
				{
                	printf("Mouse button %d released at (%d,%d)\n",
                       event.button.button, event.button.x, event.button.y);
				}*/
                break;

			case SDL_KEYUP:
				SetPressed(event.key.keysym.scancode, false);
				SetPressing(event.key.keysym.scancode, false);
				break;
			case SDL_KEYDOWN:
				if (Verbose(VerboseInfo))
				{
					printf("Pressed key %d\n", (int)event.key.keysym.scancode);
				}
				if (!KeyPressing(event.key.keysym.scancode))
				{
					SetPressed(event.key.keysym.scancode, true);
				}
				SetPressing(event.key.keysym.scancode, true);
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

		if (!Render())
			break;

		SDL_GL_SwapBuffers();

		shellFrame++;
	}
	ReleaseGL();
	ReleaseApp();

	SDL_Quit();
	return 0;             /* ANSI C requires main to return int. */
}

int main(int argc, char *argv[])
{
	shell = NewDemo();
	int ret = shell->Run(argc, argv);
	delete shell;
	return ret;
}
