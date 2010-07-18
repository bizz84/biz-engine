/*****************************************************************************
 * Filename			TTFont.h
 * 
 * License			LGPL
 *
 * Author			Andrea Bizzotto (bizz84@gmail.com)
 *
 * Platform			LinuxX11 / OpenGL
 * 
 * Description		True Type font rendering
 *
 *****************************************************************************/

#ifndef _FONT_H_
#define _FONT_H_


#ifdef __linux__
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#else
#include <SDL.h>
#include <SDL_ttf.h>
#endif

// TODO: The implementation of this class is highly inefficient but 
// serves just as a method to print out text. Implement properly
class TTFont
{
	TTF_Font *ttFont;
	char intBuffer[1024];

public:
	bool LoadFont(const char* file, int ptsize);

	static void glEnable2D();
	static void glDisable2D();

	void SDL_GL_RenderText(const char *text, 
                      SDL_Color color,
                      SDL_Rect *location) const;

	int SDL_GL_RenderText(SDL_Color color,
                      SDL_Rect *location,
					  const char *text, ...);
};

#endif

