/*****************************************************************************
 * Filename			FontTechno.h
 * 
 * License			GPLv3
 *
 * Author			Andrea Bizzotto (bizz84@gmail.com)
 *
 * Platform			LinuxX11 / OpenGL
 * 
 * Description		FontTechno
 *
 *****************************************************************************/

#ifndef _FONT_TECHNO_H_
#define _FONT_TECHNO_H_

#include "../FontManager.h"

class FontTechno : public FontManager
{
protected:
	virtual void GenerateGlyphs();
	virtual float FontAspect() const { return 0.8f; }

	virtual const char *FragmentShader() const;
public:
	FontTechno(const char *file) : FontManager(file, false) { }

	virtual void TestFont();
};

#endif
