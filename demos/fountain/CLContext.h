/*****************************************************************************
 * Filename			NBody.h
 * 
 * License			GPLv3
 *
 * Author			Andrea Bizzotto (bizz84@gmail.com)
 *
 * Platform			LinuxX11 / OpenGL
 * 
 * Description		NBody Simulation
 *
 *****************************************************************************/
#ifndef _CL_CONTEXT_H_
#define _CL_CONTEXT_H_

#if defined(__APPLE__) || defined(__MACOSX)
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif

class CLContext
{
protected:
    cl_device_id device_id;             // compute device id 
    cl_context context;                 // compute context
    cl_command_queue commands;          // compute command queue

public:
	CLContext();

	virtual bool InitCL();
	virtual bool ReleaseCL();

	cl_context GetContext() { return context; }
	cl_command_queue GetCommandQueue() { return commands; }
};


#endif