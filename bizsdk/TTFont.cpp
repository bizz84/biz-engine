/*****************************************************************************
 * Filename			TTFont.cpp
 * 
 * License			GPLv3
 *
 * Author			Andrea Bizzotto (bizz84@gmail.com)
 *
 * Platform			LinuxX11 / OpenGL
 * 
 * Description		True Type font rendering
 *
 *****************************************************************************/

#include "TTFont.h"
#include "Misc.h"
#include "Extensions.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include "SDLShell.h"


void TTFont::glEnable2D()
{
	int vPort[4];
  
	glGetIntegerv(GL_VIEWPORT, vPort);
  
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
  
	glOrtho(0, vPort[2], 0, vPort[3], -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
}

void TTFont::glDisable2D()
{
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();   
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();	
}





bool TTFont::LoadFont(const char* file, int ptsize)
{

	ttFont = TTF_OpenFont(file, ptsize);
	if (ttFont == NULL){
		printf("Unable to load font %s: %s \n", file, TTF_GetError());
		return false;
	}
	return true;

}


void TTFont::SDL_GL_RenderText(const char *text, 
                      SDL_Color color,
                      SDL_Rect *location) const
{
	SDL_Surface *initial;
	SDL_Surface *intermediary;
	int w,h;
	GLuint texture;
	
	/* Use SDL_TTF to render our text */
	initial = TTF_RenderText_Blended(ttFont, text, color);
	
	/* Convert the rendered text to a known format */
	w = NextPowerOfTwo(initial->w);
	h = NextPowerOfTwo(initial->h);
	
	intermediary = SDL_CreateRGBSurface(0, w, h, 32, 
			0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);

	SDL_BlitSurface(initial, 0, intermediary, 0);
	
	/* Tell GL about our new texture */
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, w, h, 0, GL_BGRA, 
			GL_UNSIGNED_BYTE, intermediary->pixels );
	
	/* GL_NEAREST looks horrible, if scaled... */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	

	/* prepare to render our texture */
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);
	glColor3f(1.0f, 1.0f, 1.0f);
	
	RenderQuad2D(location->x, location->y, w, h, 0.0, 0.0, 1.0, 1.0);

	
	/* Bad things happen if we delete the texture before it finishes */
	//glFinish();
	
	/* return the deltas in the unused w,h part of the rect */
	location->w = initial->w;
	location->h = initial->h;
	
	/* Clean up */
	SDL_FreeSurface(initial);
	SDL_FreeSurface(intermediary);
	glDeleteTextures(1, &texture);
}


int TTFont::SDL_GL_RenderText(SDL_Color color,
                      SDL_Rect *location,
					  const char *text, ...)
{
    int retval = 0;
    va_list va_args;
    va_start( va_args, text );
    retval = vsnprintf( intBuffer, 1024, text, va_args );
    va_end(va_args);
    SDL_GL_RenderText(intBuffer, color, location);
    return retval;

}
