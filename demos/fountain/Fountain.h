/*****************************************************************************
 * Filename			Fountain.h
 * 
 * License			GPLv3
 *
 * Author			Andrea Bizzotto (bizz84@gmail.com)
 *
 * Platform			LinuxX11 / OpenGL
 * 
 * Description		Water fountain Simulation
 *
 *****************************************************************************/
#ifndef _FOUNTAIN_H_
#define _FOUNTAIN_H_

#include "Extensions.h"
#include "SDLShell.h"
#include "Misc.h"
#include "Timer.h"
#include "Keys.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>

#include "GLResourceManager.h"
#include "VBO.h"
#include "CoordinateFrame.h"

#include "BaseGraph.h"
#include "Mesh.h"
#include "PIDController.h"

// Code for OpenCL Initialization
#include "CLContext.h"

#include "Particle.h"

#include "version.h"

class TextGraph;
class FontManager;


class Fountain : public SDLShell, public CLContext
{
	int iShowInfo;
	GLuint uiTextProgram;

	GLuint uiParticleProgram, uiCoordFrame;

    cl_program program;                 // compute program
    
	char *strFountainProgram;

	//unsigned int nParticles;
	float fPointSize;
	float camRotate;
	bool bModeChanged;

	float angle;

	enum { OPENCL_ARRAY, OPENCL_STRUCT, CPU, NUM_IMPLEMENTATIONS };
	int eMode;

	struct CommonParameters
	{
		bool bFullSteam;
		unsigned int nParticles;

		// debug timer
		float fRunTime;

	} sImpl[NUM_IMPLEMENTATIONS];
	

	/* Stride version */
	cl_kernel kernelStride;              // compute kernel

	struct Particle
	{
		cl_float4 pos;
		cl_float4 vel;
		cl_float4 hash;
	} *particle;

	ParticleVBO *pParticleVBOStride;

	cl_mem hDeviceParticle;

	int workGroupSizeStride, localGroupSizeStride;
	size_t globalSizeStride, localSizeStride;


	bool SetupStrideKernel();


	/* Array version */
    cl_kernel kernelArray;                   // compute kernel

	int workGroupSizeArray, localGroupSizeArray;
	size_t globalSizeArray, localSizeArray;

	cl_float4* pos;
	cl_float4* vel;
	cl_float4* hash;

	ParticleVBO *pParticleVBOArray;

	cl_mem hDevicePos;
	cl_mem hDeviceVel;
	cl_mem hDeviceHash;

	bool SetupArrayKernel();

	/* CPU version */
	void CPUReference(float g, float dt);


	// Debug values
	cl_float4 min;
	cl_float4 max;


	bool bDisableRendering;
	// PID Controller related variables
	bool bUsePID;
	PIDControllerGain pidController;
	int iCurPIDParam;
	void UpdatePID(float dt, unsigned int &nParticles);

	// Timing
	auto_ptr<TextGraph> pFPSGraph;
	auto_ptr<TextGraph> pPartGraph;
	auto_ptr<FontManager> pFont;
	Timer timer;


	void ParticlesInit(int n, Particle *particle);
	void ParticlesInit(int n, cl_float4 *pos, cl_float4 *vel, cl_float4 *hash);

	void DrawCoordinateFrame(float xRot, float yRot);
	bool Read(float dt, float g);

	float fDefDistance, fDistance;
	float x, y;

	// The rendering function is split into the following
	// User input and state update
	bool Input(float t, float dt);
	// OpenCL kernels execution
	bool RunCL(float t, float dt);
	// OpenGL rendering
	bool RunGL(float t, float dt);

protected:

	bool InitCL();
	bool ReleaseCL();

	// Virtuals
	virtual bool InitApp();
	virtual bool InitGL();
	virtual bool Render();
	virtual bool ReleaseGL();
	virtual bool ReleaseApp();	
	virtual bool Resize(unsigned int width, unsigned int height);

	virtual bool RequiresOpenGL2() { return true; }
	virtual bool RequiresTTF()  { return true; }

	virtual const char *GetAppName() const { return szAppName; }
	virtual const char *GetAppVersion() const { return szAppVersion; }

public:
	Fountain();
	~Fountain();
};

#endif