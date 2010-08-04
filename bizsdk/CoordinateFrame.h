/*****************************************************************************
 * Filename			CoordinateFrame.h
 * 
 * License			GPLv3
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

#ifdef __linux__
#include <SDL/SDL.h>
#else
#include <SDL.h>
#endif
#include "Extensions.h"

class CoordinateFrame
{
	GLuint uiList;

	bool bLoaded;
	CoordinateFrame();
public:
	static CoordinateFrame &Instance();

	void Make(const float scale);
	void Render() const;
	void Unload();
};

#endif

