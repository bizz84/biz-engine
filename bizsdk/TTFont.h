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


#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>


class TTFont
{
	TTF_Font *ttFont;

public:
	bool LoadFont(const char* file, int ptsize);

	static void glEnable2D();
	static void glDisable2D();

	void SDL_GL_RenderText(const char *text, 
                      SDL_Color color,
                      SDL_Rect *location);
};

#endif

