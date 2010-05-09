/*****************************************************************************
 * Filename			NBody.cpp
 * 
 * License			LGPL
 *
 * Author			Andrea Bizzotto (bizz84@gmail.com)
 *
 * Platform			LinuxX11 / OpenGL
 * 
 * Description		NBody Simulation
 *
 *****************************************************************************/

#include "CLContext.h"

#include <cstdio>
#include <cstdlib>


#include <CL/cl.hpp>

CLContext::CLContext()
{
	device_id = 0;
	context = NULL;
	commands = NULL;
}

bool CLContext::InitCL()
{
    cl_uint numPlatforms;
    cl_platform_id platform = NULL;
    int err = clGetPlatformIDs(0, NULL, &numPlatforms);
    if (err != CL_SUCCESS)
    {
        printf("Error: clGetPlatformIDs failed!\n");
        return EXIT_FAILURE;
    }
    if (0 < numPlatforms) 
    {
        cl_platform_id* platforms = new cl_platform_id[numPlatforms];
        err = clGetPlatformIDs(numPlatforms, platforms, NULL);
	    if (err != CL_SUCCESS)
		{
			printf("Error: clGetPlatformIDs failed!\n");
			return false;
		}

        for (unsigned i = 0; i < numPlatforms; ++i) 
        {
            char pbuf[100];
            err = clGetPlatformInfo(platforms[i],
                                       CL_PLATFORM_VENDOR,
                                       sizeof(pbuf),
                                       pbuf,
                                       NULL);

			if (err != CL_SUCCESS)
			{
				printf("Error: clGetPlatformInfo failed!\n");
                return false;
            }


            platform = platforms[i];
            if (!strcmp(pbuf, "Advanced Micro Devices, Inc.")) 
            {
                break;
            }
        }
        delete[] platforms;
    }

    if( NULL == platform)
    {
        printf("NULL platform found so Exiting Application.");
        return false;
    }

    /*
     * If we could find our platform, use it.
     * Otherwise use just available platform.
     */

	// TODO: Make platform-independent
    cl_context_properties cps[7] = 
    {
        CL_CONTEXT_PLATFORM, 
        (cl_context_properties)platform, 
		CL_GL_CONTEXT_KHR,
		(cl_context_properties)wglGetCurrentContext(),
		CL_WGL_HDC_KHR,
		(cl_context_properties)wglGetCurrentDC(),
        0
    };

	cl_device_type dType = CL_DEVICE_TYPE_GPU;

	/*   cl_context_properties cl_properties[] = {
      CL_CONTEXT_PLATFORM, (cl_context_properties)platform,
      CL_GL_CONTEXT_KHR, (cl_context_properties)wglGetCurrentContext(),
      CL_WGL_HDC_KHR, (cl_context_properties)wglGetCurrentDC(),
      0};
   cl_c = clCreateContextFromType(cl_properties, CL_DEVICE_TYPE_GPU, NULL, NULL, &err_code); */

    context = clCreateContextFromType(
                  cps,
                  dType,
                  NULL,
                  NULL,
                  &err);


    // Connect to a compute device
    //
    err = clGetDeviceIDs(platform, dType, 1, &device_id, NULL);
    if (err != CL_SUCCESS)
    {
        printf("Error: Failed to create a device group!\n");
        return false;
    }
  
    // Create a compute context 
    //
    context = clCreateContext(0, 1, &device_id, NULL, NULL, &err);
    if (!context)
    {
        printf("Error: Failed to create a compute context!\n");
        return false;
    }

    // Create a command commands
    //
    commands = clCreateCommandQueue(context, device_id, 0, &err);
    if (!commands)
    {
        printf("Error: Failed to create a command commands!\n");
        return false;
    }

	return true;
}

bool CLContext::ReleaseCL()
{
    clReleaseCommandQueue(commands);
    clReleaseContext(context);
	return true;
}
