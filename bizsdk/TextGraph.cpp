/*****************************************************************************
 * Filename			CoordinateFrame.h
 * 
 * License			GPLv3
 *
 * Author			Andrea Bizzotto (bizz84@gmail.com)
 *
 * Platform			LinuxX11 / OpenGL
 * 
 * Description		Class for drawing graphs
 *
 *****************************************************************************/

#include "TextGraph.h"
#include "FontManager.h"

static const float z = 0.0f;
static const float u = 1.0f;
static const float v = 1.0f;

static const float green[] = { z, v, z, u };
static const float red[] = { v, z, z, u };
static const float white[] = { v, v, v, u };
static const float yellow[] = { v, v, z, u };

bool TextGraph::Init(bool showMax, bool showMin, bool showAvg, bool showCur,
		const char *precision, float textScale, float timeFrame, unsigned int samples, 
		float x0, float y0, float x1, float y1, bool negative, float max/* = 0.0f*/)
{
	if (!BaseGraph::Init(timeFrame, samples, x0, y0, x1, y1, negative, max))
		return false;

	bShowMax = showMax;
	bShowMin = showMin;
	bShowAvg = showAvg;
	bShowCur = showCur;

	fTextScale = textScale;

	strcpy(szPrecision, precision);
}


void TextGraph::TextDraw(const FontManager *font) const
{
	if (bShowMax)
	{
		font->Render(fX1, fY1 + 0.01f, fTextScale, (float *)green, FontManager::RightAlign,
			FontManager::BottomAlign, szPrecision, fCurrentMax);
	}

	if (bShowMin)
	{
		font->Render(fX1, fY0 - 0.01f, fTextScale, (float *)green, FontManager::RightAlign,
			FontManager::TopAlign, szPrecision, fCurrentMin);
	}
	if (bShowAvg)
	{
		font->Render(fX0, fY0 - 0.01f, fTextScale, (float *)yellow, FontManager::LeftAlign,
			FontManager::TopAlign, szPrecision, fCurrentSum / (float)uiCurSamples);
	}
	if (bShowCur)
	{
		font->Render(fX0, fY1 + 0.01f, fTextScale, (float *)white, FontManager::LeftAlign,
			FontManager::BottomAlign, szPrecision, fCurValue);
	}
}