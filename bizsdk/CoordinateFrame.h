/*****************************************************************************
 * Filename			CoordinateFrame.h
 * 
 * License			LGPL
 *
 * Author			Andrea Bizzotto (bizz84@gmail.com)
 *
 * Platform			LinuxX11 / OpenGL
 * 
 * Description		High resolution CoordinateFrame
 *
 *****************************************************************************/

#ifndef _COORDINATE_FRAME_H_
#define _COORDINATE_FRAME_H_

#include <SDL/SDL.h>
#include "Extensions.h"

class CoordinateFrame
{
	GLuint uiList;

	bool bLoaded;
	CoordinateFrame();
public:
	static CoordinateFrame &Instance();

	void Make(const float scale);
	void Render();
	void Unload();
};

#endif

