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

#include "BaseGraph.h"
#include "GLResourceManager.h"


static const float red[] = { 1.0f, 0.0f, 0.0f, 1.0f };
static const float yellow[] = { 1.0f, 1.0f, 0.0f, 1.0f };
static const float white[] = { 1.0f, 1.0f, 1.0f, 1.0f };

static const float c_fBigNumber = 1000000000.0f;

BaseGraph::BaseGraph()
{
	fTimeFrame = 0.0f;
	uiSamples = 100;
	fX0 = -1.0f;
	fY0 = -1.0f;
	fX1 = 1.0f;
	fY1 = 1.0f;
	bNegative = false;
	fMax = 0.0f;

	afScale[0] = afScale[1] = 1.0f;

	fCurrentMax = 0.0f;
	fCurrentMin = 0.0f;
	fCurrentSum = 0.0f;
	bOverflow = false;
	bUnderflow = false;

	psSamples = NULL;
	pvSamples = NULL;
	uiCurrentSample = 0U;
	uiLastSample = 0U;
	uiCurSamples = 0U;

	fCurValue = 0.0f;
	fCurTime = 0.0f;
}

BaseGraph::~BaseGraph()
{
	delete [] psSamples;
	delete [] pvSamples;
}

bool BaseGraph::Init(float timeFrame, unsigned int samples, 
		float x0, float y0,  float x1, float y1, bool negative, float max/* = 0.0f*/)
{
	fTimeFrame = timeFrame;
	uiSamples = samples;
	afTransition[0] = fX0 = x0;
	afTransition[1] = fY0 = y0;
	fX1 = x1;
	fY1 = y1;
	bNegative = negative;
	fMax = max;	

	afScale[0] = (fX1 - fX0);
	afScale[1] = (fY1 - fY0);	

	psSamples = new BaseGraphSample[uiSamples];
	pvSamples = new GraphSample[uiSamples];

	memset(psSamples, 0, sizeof(BaseGraphSample) * uiSamples);
	memset(pvSamples, 0, sizeof(GraphSample) * uiSamples);

	if (bNegative)
	{
		afScale[1] *= 0.5f;
		afTransition[1] = 0.5f * (y0 + y1);
	}

	// Load shaders
	GLResourceManager &loader = GLResourceManager::Instance();
	
	// Load Border shader
	const char* borderVertCode = "\
					 attribute  vec2  inVertex;\
					 void main(void)\
					 {\
						gl_Position = vec4(inVertex, 0.0, 1.0);\
					 }";

	const char* borderFragCode = "\
					 uniform vec4 Color;\
					 void main(void)\
					 {\
						gl_FragColor =  vec4(Color);\
					 }";

	if (!loader.LoadShaderFromMemory(borderVertCode, borderFragCode, sBorderGraphShader.uiID))
	{
		return false;
	}

	sBorderGraphShader.uiColourLoc = glGetUniformLocation(sBorderGraphShader.uiID, "Color");

	// Load data shader
	const char* dataVertCode = "\
					 attribute  vec2  inVertex;\
					 uniform float Max; \
					 uniform float Min; \
					 uniform vec2 Scale;\
					 uniform vec2 Transition;\
					 void main(void)\
					 {\
						vec2 pos = vec2(max(inVertex.x, 0.0), clamp(inVertex.y / Max, Min, 1.0));\
						vec2 scaled = pos * Scale + Transition; \
						gl_Position = vec4(scaled, 0.0, 1.0);\
					 }";

	const char* dataFragCode = "\
					 uniform vec4 Color;\
					 void main(void)\
					 {\
						gl_FragColor =  vec4(Color);\
					 }";

	if (!loader.LoadShaderFromMemory(dataVertCode, dataFragCode, sGraphShader.uiID))
	{
		return false;
	}

	sGraphShader.uiColourLoc	= glGetUniformLocation(sGraphShader.uiID, "Color");

	sGraphShader.uiMaxLoc	= glGetUniformLocation(sGraphShader.uiID, "Max");
	sGraphShader.uiMinLoc	= glGetUniformLocation(sGraphShader.uiID, "Min");
	sGraphShader.uiScaleLoc	= glGetUniformLocation(sGraphShader.uiID, "Scale");
	sGraphShader.uiTransitionLoc	= glGetUniformLocation(sGraphShader.uiID, "Transition");

	float afAttribs[] = {
		x0, y0,
		x1, y0,
		x1, y1,
		x0, y1,
	};
	memcpy(afVertexAttribs, afAttribs, sizeof(float) * 8);

	afVertexAttribsAxis[0] = x0;
	afVertexAttribsAxis[1] = 0.5f * (y1 + y0);
	afVertexAttribsAxis[2] = x1;
	afVertexAttribsAxis[3] = 0.5f * (y1 + y0);

	return true;
}

void BaseGraph::Input(float value, float time/* = 0.0f*/)
{
	psSamples[uiCurrentSample].value = value;
	psSamples[uiCurrentSample].time = time;

	fCurValue = value;
	fCurTime = time;

	unsigned int i;
	for (i = 0; uiLastSample != uiCurrentSample; uiLastSample = (uiLastSample == uiSamples - 1 ? 0 : uiLastSample + 1), i++)
	{
		if (psSamples[uiLastSample].time >= time - fTimeFrame)
			break;
	}
	if (i > 0)
	{
		uiLastSample = uiLastSample == 0 ? uiSamples - 1 : uiLastSample - 1;
	}

	uiCurrentSample = uiCurrentSample == uiSamples - 1 ? 0 : uiCurrentSample + 1;

	// Update state
	fCurrentMax = -c_fBigNumber;
	fCurrentMin = c_fBigNumber;
	fCurrentSum = 0.0f;
	unsigned int cur;
	for (uiCurSamples = 0, cur = uiLastSample; cur != uiCurrentSample; cur = (cur == uiSamples - 1 ? 0 : cur + 1), uiCurSamples++)
	{	
		fCurrentSum += psSamples[cur].value;
		if (psSamples[cur].value > fCurrentMax)
			fCurrentMax = psSamples[cur].value;
		if (psSamples[cur].value < fCurrentMin)
			fCurrentMin = psSamples[cur].value;

		pvSamples[uiCurSamples].x = 1.0f - (fCurTime - psSamples[cur].time) / fTimeFrame;
		pvSamples[uiCurSamples].y = psSamples[cur].value;
	}
	if (fMax != 0.0f)
	{
		bOverflow = fCurrentMax > fMax;
		if (bNegative)
			bUnderflow = fCurrentMin < -fMax;
		else
			bUnderflow = fCurrentMin < 0.0f;
	}
}

void BaseGraph::Draw()
{
	glEnableVertexAttribArray(0);

	// Draw Border
	glUseProgram(sBorderGraphShader.uiID);

	glUniform4fv(sBorderGraphShader.uiColourLoc, 1, red);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, &afVertexAttribs);

	glDrawArrays(GL_LINE_LOOP, 0, 4);

	// Draw Axis
	if (bNegative)
	{
		glUniform4fv(sBorderGraphShader.uiColourLoc, 1, white);		

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, &afVertexAttribsAxis);

		glDrawArrays(GL_LINE_STRIP, 0, 2);
	}

	// Draw data
	glUseProgram(sGraphShader.uiID);

	glUniform4fv(sGraphShader.uiColourLoc, 1, yellow);

	float max;
	if (fMax == 0.0f)
		max = fCurrentMax > -fCurrentMin ? fCurrentMax : -fCurrentMin;
	else
		max = fMax;
	glUniform1f(sGraphShader.uiMaxLoc, max);
	glUniform1f(sGraphShader.uiMinLoc, bNegative ? -1.0f : 0.0f);
	glUniform2fv(sGraphShader.uiScaleLoc, 1, afScale);
	glUniform2fv(sGraphShader.uiTransitionLoc, 1, afTransition);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, &pvSamples[0].x);

	glDrawArrays(GL_LINE_STRIP, 0, uiCurSamples);

	glDisableVertexAttribArray(0);
}	
