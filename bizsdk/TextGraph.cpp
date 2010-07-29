/*****************************************************************************
 * Filename			CoordinateFrame.h
 * 
 * License			LGPL
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


static const float green[] = { 0.0f, 1.0f, 0.0f, 1.0f };
static const float red[] = { 1.0f, 0.0f, 0.0f, 1.0f };
static const float white[] = { 1.0f, 1.0f, 1.0f, 1.0f };
static const float yellow[] = { 1.0f, 1.0f, 0.0f, 1.0f };

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
		font->Render(fX1, fY1, fTextScale, (float *)green, FontManager::RightAlign,
			FontManager::BottomAlign, szPrecision, fCurrentMax);
	}

	if (bShowMin)
	{
		font->Render(fX1, fY0, fTextScale, (float *)green, FontManager::RightAlign,
			FontManager::TopAlign, szPrecision, fCurrentMin);
	}
	if (bShowAvg)
	{
		font->Render(fX0, fY0, fTextScale, (float *)yellow, FontManager::LeftAlign,
			FontManager::TopAlign, szPrecision, fCurrentSum / (float)uiCurSamples);
	}
	if (bShowCur)
	{
		font->Render(fX0, fY1, fTextScale, (float *)white, FontManager::LeftAlign,
			FontManager::BottomAlign, szPrecision, fCurValue);
	}
}