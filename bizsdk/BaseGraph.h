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

#ifndef _BASE_GRAPH_H_
#define _BASE_GRAPH_H_

#include "Extensions.h"

class BaseGraph
{
protected:
	struct BaseGraphShader {
		GLuint uiID;
		GLuint uiColourLoc;
	} sBorderGraphShader;
	
	
	struct GraphSample
	{
		float x;
		float y;
	} *pvSamples;

	struct BaseGraphSample
	{
		float value;
		float time;
	} *psSamples;

	struct DataGraphShader {
		GLuint uiID;
		GLuint uiColourLoc;
		GLuint uiMaxLoc;
		GLuint uiMinLoc;
		GLuint uiScaleLoc;
		GLuint uiTransitionLoc;
	} sGraphShader;

	float fTimeFrame;
	unsigned int uiSamples;
	float fX0, fY0, fX1, fY1;
	bool bNegative;
	float fMax;

	float afScale[2];
	float afTransition[2];

	float afVertexAttribs[8];
	float afVertexAttribsAxis[4];

	unsigned int uiCurrentSample;
	unsigned int uiLastSample;

	float fCurrentMax;
	float fCurrentMin;
	float fCurrentSum;
	unsigned int uiCurSamples;

	float fCurValue;
	float fCurTime;

	// Can be true only if fMax != 0
	bool bOverflow;
	bool bUnderflow;

public:
	//! Constructor.
	BaseGraph();

	//! Destructor
	virtual ~BaseGraph();

	bool Init(float timeFrame, unsigned int samples, 
		float x0, float y0, float x1, float y1, bool negative, float max = 0.0f);

	void Input(float value, float time = 0.0f);

	void Draw() const;

	const bool IsOverflow() const { return bOverflow; }
	const bool IsUnderflow() const { return bUnderflow; }

	const float GetCurrent() const { return fCurValue; }
	const float GetMax() const { return fCurrentMax; }
	const float GetMin() const { return fCurrentMin; }
	const float GetAverage() const { return fCurrentSum / uiCurSamples; }
	const float GetTime() const { return fCurTime; }	
};

#endif