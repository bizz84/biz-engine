/*****************************************************************************
 * Filename			Fountain.h
 * 
 * License			LGPL
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
#include "TTFont.h"
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


class Fountain : public SDLShell, public CLContext
{
	int iShowInfo;
	TTFont ttFont;
	GLuint uiTextProgram;

	GLuint uiParticleProgram, uiCoordFrame;

	ParticleVBO *pParticleVBO;

    cl_program program;                 // compute program
    cl_kernel kernel;                   // compute kernel
    
	char *strFountainKernel;

	unsigned int nParticles;
	float fPointSize;
	float camRotate;

	float angle;

	cl_float4* pos;
	cl_float4* vel;
	cl_float4* hash;

	cl_float4 min;
	cl_float4 max;

	cl_mem hDevicePos;
	cl_mem hDeviceVel;
	cl_mem hDeviceHash;

	int workGroupSize, localGroupSize;
	size_t globalSize, localSize;

	// PID Controller related variables
	bool bUsePID;
	PIDControllerGain pidController;
	int iCurPIDParam;
	void UpdatePID(float dt);

	// Timing
	BaseGraph fpsGraph;
	Timer timer;


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

	virtual const char *GetAppName() { return szAppName; }
	virtual const char *GetAppVersion()  { return szAppVersion; }

public:
	Fountain();
	~Fountain();
};

#endif