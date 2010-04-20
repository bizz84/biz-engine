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

#ifndef _BASE_GRAPH_H_
#define _BASE_GRAPH_H_

#include "Extensions.h"

class BaseGraph
{
protected:
	float fTimeFrame;
	unsigned int uiSamples;
	float fX0, fY0, fX1, fY1;
	bool bNegative;
	float fMax;

	float afScale[2];
	float afTransition[2];

	struct BaseGraphShader {
		GLuint uiID;
		GLuint uiColourLoc;
	} sBorderGraphShader;

	struct DataGraphShader {
		GLuint uiID;
		GLuint uiColourLoc;
		GLuint uiMaxLoc;
		GLuint uiMinLoc;
		GLuint uiScaleLoc;
		GLuint uiTransitionLoc;
	} sGraphShader;

	float afVertexAttribs[8];
	float afVertexAttribsAxis[4];

	struct GraphSample
	{
		float x;
		float y;
	};
	GraphSample *pvSamples;

	struct BaseGraphSample
	{
		float value;
		float time;
	} *psSamples;
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

	//! Deconstructor.
	virtual ~BaseGraph();

	bool Init(float timeFrame, unsigned int samples, 
		float x0, float y0,  float x1, float y1, bool negative, float max = 0.0f);

	virtual void Input(float value, float time = 0.0f);

	virtual void Draw();

	bool IsOverflow() { return bOverflow; }
	bool IsUnderflow() { return bUnderflow; }

	float GetCurrent() { return fCurValue; }
	float GetMax() { return fCurrentMax; }
	float GetMin() { return fCurrentMin; }
	float GetAverage() { return fCurrentSum / uiCurSamples; }
	float GetTime() { return fCurTime; }	
};

#endif