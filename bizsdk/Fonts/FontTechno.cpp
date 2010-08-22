/*****************************************************************************
 * Filename			FontTechno.cpp
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

#include "FontTechno.h"



void FontTechno::GenerateGlyphs()
{
	unsigned char c;
	const float yoff = 0.02f;
	const float ydim = 0.07f;

	const float cxoff = 0.02f;
	const float cxdim = 0.06f;
	const float sxoff = 0.02f;
	const float sxdim = 0.06f;
	const float nxoff = 0.025f;
	const float nxdim = 0.05f;
	// Capital letters are in row 0
	for (c = 'A'; c <= 'J'; c++)
	{
		glyph[c] = Glyph((float)(c - 'A') * 0.1f + cxoff, 0.0f + yoff, cxdim, ydim);
	}
	for (c = 'K'; c <= 'T'; c++)
	{
		glyph[c] = Glyph((float)(c - 'K') * 0.1f + cxoff, 0.1f + yoff, cxdim, ydim);
	}
	for (c = 'U'; c <= 'Z'; c++)
	{
		glyph[c] = Glyph((float)(c - 'U') * 0.1f + cxoff, 0.2f + yoff, cxdim, ydim);
	}

	// Small letters are in row 3
	for (c = 'a'; c <= 'j'; c++)
	{
		glyph[c] = Glyph((float)(c - 'a') * 0.1f + sxoff, 0.3f + yoff, sxdim, ydim);
	}
	for (c = 'k'; c <= 't'; c++)
	{
		glyph[c] = Glyph((float)(c - 'k') * 0.1f + sxoff, 0.4f + yoff, sxdim, ydim);
	}
	for (c = 'u'; c <= 'z'; c++)
	{
		glyph[c] = Glyph((float)(c - 'u') * 0.1f + sxoff, 0.5f + yoff, sxdim, ydim);
	}
	// Numers are in row 6
	for (c = '0'; c <= '9'; c++)
	{
		glyph[c] = Glyph((float)(c - '0') * 0.1f + nxoff, 0.6f + yoff, nxdim, ydim);
	}

	const char punct1[] = ",.[]()+-*/";
	const char punct2[] = "~?!=";
	unsigned int i;
	for (i = 0; i < 10; i++)
	{
		glyph[punct1[i]] = Glyph((float)i * 0.1f + nxoff, 0.7f + yoff, nxdim, ydim);
	}
	for (i = 0; i < 4; i++)
	{
		glyph[punct2[i]] = Glyph((float)i * 0.1f + nxoff, 0.8f + yoff, nxdim, ydim);
	}
	glyph['.'] = Glyph(1.0f * 0.1f + 0.04f, 0.7f + yoff, 0.02f, ydim);
}

void FontTechno::TestFont()
{
	float red[] = {1.0,0.0,0.0,1.0};
	float blue[] = {0.0,0.0,1.0,1.0};
	float magenta[] = {1.0,0.0,1.0,1.0};
	float cyan[] = {0.0,1.0,1.0,1.0};
	float orange[] = {1.0,1.0,0.0,1.0};
	
	Render(0.0, 0.0, 0.12f, red, FontManager::CenterAlign, FontManager::MiddleAlign, "abcdefghijklm");
	Render(0.0, -0.15, 0.12f, blue, FontManager::CenterAlign, FontManager::MiddleAlign, "nopqrstuvxwyz");
	Render(0.0, -0.3, 0.12f, magenta, FontManager::CenterAlign, FontManager::MiddleAlign, "ABCDEFGHIJKLM");
	Render(0.0, -0.45, 0.12f, cyan, FontManager::CenterAlign, FontManager::MiddleAlign, "NOPQRSTUVXWYZ");
	Render(0.0, -0.6, 0.12f, orange, FontManager::CenterAlign, FontManager::MiddleAlign, "0123456789");
}