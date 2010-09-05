/*****************************************************************************
 * Filename			FontManager.cpp
 * 
 * License			GPLv3
 *
 * Author			Andrea Bizzotto (bizz84@gmail.com)
 *
 * Platform			LinuxX11 / OpenGL
 * 
 * Description		Font Manager
 *
 *****************************************************************************/

#include "FontManager.h"
#include "GLResourceManager.h"

FontManager::FontManager(const char *file, bool fixed)
	: textureFile(file), fixedWidth(fixed)
{
	for (unsigned int i = 0; i < NUM_GLYPHS; i++)
	{
		aiGlyphLookup[i] = i; // empty glyph
	}

}

bool FontManager::LoadShaders()
{
	const char vertexShader[] = \
		"varying vec2 TexCoord;" \
		"void main()" \
		"{" \
			"gl_Position = vec4(gl_Vertex.x, gl_Vertex.y, 0.0, 1.0);" \
			"TexCoord = gl_MultiTexCoord0.xy;" \
		"}";

	GLResourceManager &loader = GLResourceManager::Instance();
	if (!loader.LoadShaderFromMemory(vertexShader, FragmentShader(), program))
		return false;
	
	glUseProgram(program);
	glUniform1i(GetUniLoc(program, "sTexture"), 0);
	locColor = GetUniLoc(program, "Color");

	return true;
}

bool FontManager::LoadTexture()
{
	GLResourceManager &loader = GLResourceManager::Instance();
	if (!loader.LoadTextureFromFile(textureFile.c_str(), texture, GL_LINEAR, GL_LINEAR))
		return false;

	return true;
}

bool FontManager::Init()
{
	if (!LoadShaders())
		return false;

	if (!LoadTexture())
		return false;

	GenerateGlyphs();
	return true;
}


void FontManager::Bind() const
{
	glBindTexture(GL_TEXTURE_2D, texture);
	glUseProgram(program);	
}

const float FontManager::Width(const char *str, float height) const
{
	int len = strlen(str);
	if (FixedWidth())
		return len * height;

	float width = 0.0f;

	unsigned int i;
	for (i = 0; i < len; i++)
	{
		unsigned char c = str[i];
		const Glyph &g = glyph[GlyphIndex(str[i])];
		width += g.Ratio(FontAspect()) * height;
	}
	return width;
}

void FontManager::Render(float posX, float posY, float height, float *color,
	HorzAlign halign, VertAlign valign, const char *text, ...) const
{
	// format string
	char str[1024];
	va_list va_args;
    va_start( va_args, text );
    int retval = vsnprintf( str, 1024, text, va_args );
    va_end(va_args);

	// Adjust to aspect ratio
	float width = Width(str, height);
	float startX = posX;
	float startY = posY;
	if (halign == CenterAlign)
		startX -= 0.5f * width;
	else if (halign == RightAlign)
		startX -= width;
	
	if (valign == MiddleAlign)
		startY += 0.5f * height;
	else if (valign == BottomAlign)
		startY += height;

	// tex coord multiplier
	float h = height;
		
	// each vertex = [x,y,u,v], 4 vertices per glyph
	int len = strlen(str);
	float *attribs = new float[4 * 4 * len];
	float *a = attribs;	
	
	unsigned int i;
	for (i = 0; i < len; i++)
	{
		int index = GlyphIndex(str[i]);
		const Glyph &g = glyph[index];
		float w = g.Ratio(FontAspect()) * height;
		*a++ = startX;
		*a++ = startY;
		*a++ = g.x;
		*a++ = g.y;

		*a++ = startX + w;
		*a++ = startY;
		*a++ = g.x + g.w;
		*a++ = g.y;

		*a++ = startX + w;
		*a++ = startY - h;
		*a++ = g.x + g.w;
		*a++ = g.y + g.h;

		*a++ = startX;
		*a++ = startY - h;
		*a++ = g.x;
		*a++ = g.y + g.h;
		
		startX += w;
	}
	
	glUniform4fv(locColor, 1, color);

	glVertexPointer(2, GL_FLOAT, sizeof(float) * 4, attribs);
	glTexCoordPointer(2, GL_FLOAT, sizeof(float) * 4, attribs + 2);
	glDrawArrays(GL_QUADS, 0, len * 4);
}

