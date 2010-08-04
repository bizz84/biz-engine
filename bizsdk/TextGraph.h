/*****************************************************************************
 * Filename			BaseGraph.h
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

#ifndef _TEXT_GRAPH_H_
#define _TEXT_GRAPH_H_

#include "BaseGraph.h"

class FontManager;

class TextGraph : public BaseGraph
{
	bool bShowMax, bShowMin, bShowAvg, bShowCur;
	char szPrecision[10];
	float fTextScale;
public:

	bool Init(bool showMax, bool showMin, bool showAvg, bool showCur,
		const char *precision, float textScale, float timeFrame, unsigned int samples, 
		float x0, float y0, float x1, float y1, bool negative, float max = 0.0f);

	void TextDraw(const FontManager *font) const;
};

#endif