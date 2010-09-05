/*****************************************************************************
 * Filename			FontManager.h
 * 
 * License			GPLv3
 *
 * Author			Andrea Bizzotto (bizz84@gmail.com)
 *
 * Platform			LinuxX11 / OpenGL
 * 
 * Description		FontManager
 *
 *****************************************************************************/

#ifndef _FONT_GOTHIC_H_
#define _FONT_GOTHIC_H_

#include "../FontManager.h"

class FontGothic : public FontManager
{
protected:
	virtual void GenerateGlyphs();

	virtual const char *FragmentShader() const;

public:
	FontGothic(const char *file) : FontManager(file, false) { }
};

#endif
