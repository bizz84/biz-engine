/*****************************************************************************
 * Filename			FontManager.h
 * 
 * License			LGPL
 *
 * Author			Andrea Bizzotto (bizz84@gmail.com)
 *
 * Platform			LinuxX11 / OpenGL
 * 
 * Description		FontManager
 *
 *****************************************************************************/

#ifndef _FONT_MANAGER_H_
#define _FONT_MANAGER_H_

#include "Extensions.h"

#include <string>
using namespace std;

// Simple font manager class. Not particularly well designed, supports only
// one Font type stored in a texture
// No caching or other techiques are used to increase efficiency
class FontManager
{
protected:
	class Glyph
	{
	public:
		// texture coordinates
		float x, y, w, h;

		Glyph() : x(0.0f), y(0.0f), w(0.0f), h(0.0f) { }
		Glyph(float x, float y, float w, float h)
			: x(x), y(y), w(w), h(h) { }
		Glyph(int x0, int y0, int x1, int y1, int texW, int texH)
			: x((float)x0 / (float)texW), y((float)y0 / (float)texH),
			w((float)(x1 - x0) / (float)texW), h((float)(y1 - y0) / (float)texH) { }

		const float Width() const { return w; }
		const float Height() const { return h; }

		const float Ratio(float intrinsic = 1.0f) const { return h == 0.0 ? intrinsic : intrinsic * w / h; }
	};
	
	enum { NUM_GLYPHS = 128 };
	int aiGlyphLookup[NUM_GLYPHS];
	Glyph glyph[NUM_GLYPHS];

	bool fixedWidth;

	string textureFile;
	
	GLuint program;
	GLuint texture;
	GLint locColor;

	virtual void GenerateGlyphs() = 0;
	virtual float FontAspect() const { return 1.0f; }
	bool LoadShaders();
	bool LoadTexture();
public:
	FontManager(const char *file, bool fixed);
	virtual ~FontManager() { }

	bool Init();
	
	void Bind() const;

	const float Width(const char *str, float height) const;

	enum HorzAlign { LeftAlign, CenterAlign, RightAlign };
	enum VertAlign { TopAlign, MiddleAlign, BottomAlign };
	//! Render: this function formats the input string via va_list, creates a
	// a vertex buffer and passes it to GL for rendering
	void Render(float posX, float posY, float height, float *color,
		HorzAlign halign, VertAlign valign, const char *text, ...) const;

	const int GlyphIndex(unsigned char ch) const { return aiGlyphLookup[ch]; }	

	const bool FixedWidth() const { return fixedWidth; }

	virtual void TestFont() { }

};


#endif
