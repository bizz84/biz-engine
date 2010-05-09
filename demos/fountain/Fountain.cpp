/*****************************************************************************
 * Filename			Fountain.cpp
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

#include "Fountain.h"

#include <CL/cl.hpp>


#define BUFFERS_SIZE (1 << 19)
#define MIN_SIZE (1 << 8)

#define TARGET_FRAMERATE 30.0f

Fountain::Fountain()
{
	pos = vel = NULL;
	hash = NULL;
	strFountainProgram = NULL;

	pParticleVBOArray = NULL;
	pParticleVBOStride = NULL;
}

Fountain::~Fountain()
{

}

bool Fountain::InitApp()
{
	// Standard initialization
	fTimeArray = 0.0f;
	fTimeStride = 0.0f;

	bDisableRendering = false;

	bUseStrideKernel = false;

	// Initialization for command line modifiable variables
	iShowInfo = 0;
	bUsePID = false;

	// Custom command line processing
	vector<CmdLineParameter>::iterator iter;
	vector<CmdLineParameter> &params = GetCommandLineParams();

	for (iter = params.begin(); iter != params.end(); iter++)
	{
		if (iter->sName == "showinfo")
		{
			const char *str = iter->sValue.c_str();
			iShowInfo = atoi(str);
		}
		if (iter->sName == "usepid")
		{
			const char *str = iter->sValue.c_str();
			bUsePID = (bool)atoi(str);
		}
	}	

	return true;
}

bool Fountain::InitCL()
{
	if (!CLContext::InitCL())
		return false;

	int err;

    nParticles = BUFFERS_SIZE; /* MUST be a nice power of two for simplicity */
	// Load kernelArray from file
	if (LoadFileIntoMemory("data/kernels/fountain.cl", &strFountainProgram) < 0)
		return false;

	// Create the compute program from the source buffer
    program = clCreateProgramWithSource(context, 1, (const char **) & strFountainProgram, NULL, &err);
    if (!program)
    {
        printf("Error: Failed to create compute program!\n");
        return false;
    }

    // Build the program executable
    err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
    if (err != CL_SUCCESS)
    {
        size_t len;
        char buffer[2048];

        printf("Error: Failed to build program executable!\n");
        clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
        printf("%s\n", buffer);
        return false;
    }
	if (!SetupArrayKernel())
	{
		return false;
	}
	if (!SetupStrideKernel())
	{
		return false;
	}
	return true;
}

bool Fountain::SetupStrideKernel()
{
	int err;
	/* ------------------------  Stride version  -------------------------------- */
	nParticlesStride = nParticles;

	// Create the compute kernelArray in the program we wish to run
    kernelStride = clCreateKernel(program, "fountain_kern_stride", &err);
    if (!kernelStride || err != CL_SUCCESS)
    {
        printf("Error: Failed to create compute kernelArray!\n");
        return false;
    }

	// Get work group size for kernelArray
	err = clGetKernelWorkGroupInfo(kernelStride, device_id, CL_KERNEL_WORK_GROUP_SIZE, sizeof(workGroupSizeStride), &workGroupSizeStride, NULL);
    if (err != CL_SUCCESS)
    {
        printf("Error: Failed to retrieve kernelArray work group info! %d\n", err);
        return false;
    }
    localGroupSizeStride = workGroupSizeStride / sizeof(Particle);

	// Other initialization
	localSizeStride = localSizeArray;//localGroupSizeStride * sizeof(Particle);
	globalSizeStride = globalSizeArray;//BUFFERS_SIZE * sizeof(Particle);


	particle = new Particle[BUFFERS_SIZE];
	ParticlesInit(BUFFERS_SIZE, particle);

	pParticleVBOStride = new ParticleVBO(particle, sizeof(Particle), BUFFERS_SIZE);

	//hDeviceParticle = clCreateBuffer(GetContext(),
	//	CL_MEM_READ_WRITE|CL_MEM_COPY_HOST_PTR,
	//	BUFFERS_SIZE*sizeof(cl_float4), particle, NULL);

	hDeviceParticle = clCreateFromGLBuffer(GetContext(),
		CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,//CL_MEM_ALLOC_HOST_PTR,
		pParticleVBOStride->GetVBO(), &err);

	if (!hDeviceParticle)
    {
        printf("Error: Failed to create buffers!\n");
		return false;
	}

	err = 0;
	// set attributes
	err |= clSetKernelArg(kernelStride, 0, sizeof(cl_mem), (void *)&hDeviceParticle);

    if (err != CL_SUCCESS)
    {
        printf("Error: Failed to set kernelArray arguments! %d\n", err);
        return false;
    }
	return true;
}

bool Fountain::SetupArrayKernel()
{
	int err;
	/* ------------------------  Array version  -------------------------------- */
	nParticlesArray = nParticles;
	
	// Create the compute kernelArray in the program we wish to run
    kernelArray = clCreateKernel(program, "fountain_kern_array", &err);
    if (!kernelArray || err != CL_SUCCESS)
    {
        printf("Error: Failed to create compute kernelArray!\n");
        return false;
    }

	// Get work group size for kernelArray
	err = clGetKernelWorkGroupInfo(kernelArray, device_id, CL_KERNEL_WORK_GROUP_SIZE, sizeof(workGroupSizeArray), &workGroupSizeArray, NULL);
    if (err != CL_SUCCESS)
    {
        printf("Error: Failed to retrieve kernelArray work group info! %d\n", err);
        return false;
    }
    localGroupSizeArray = workGroupSizeArray / sizeof(cl_float4);

	// Other initialization
	localSizeArray = localGroupSizeArray * sizeof(cl_float4);
	globalSizeArray = BUFFERS_SIZE * sizeof(cl_float4);


	// Create host buffers 

	pos = new cl_float4[BUFFERS_SIZE];
	vel = new cl_float4[BUFFERS_SIZE];
	hash = new cl_float4[BUFFERS_SIZE];

	// Initialize client vectors
	ParticlesInit(BUFFERS_SIZE, pos, vel, hash);

	// Init Velocity buffer
	hDeviceVel = clCreateBuffer(GetContext(),
		CL_MEM_READ_WRITE|CL_MEM_COPY_HOST_PTR,
		BUFFERS_SIZE*sizeof(cl_float4), vel, NULL);

	// Init Hash (random) buffer
	hDeviceHash = clCreateBuffer(GetContext(),
		CL_MEM_READ_WRITE|CL_MEM_COPY_HOST_PTR,
		BUFFERS_SIZE*sizeof(cl_float4), hash, NULL);

	pParticleVBOArray = new ParticleVBO(pos, sizeof(cl_float4), BUFFERS_SIZE);

	hDevicePos = clCreateFromGLBuffer(GetContext(),
		CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,//CL_MEM_ALLOC_HOST_PTR,
		pParticleVBOArray->GetVBO(), &err);


	// Check for correctness
	if (!hDevicePos || !hDeviceVel || !hDeviceHash)
    {
        printf("Error: Failed to create buffers!\n");
		return false;
	}
	// Set Parameters

	err = 0;
	// set attributes
	err |= clSetKernelArg(kernelArray, 0, sizeof(cl_mem), (void *)&hDevicePos);
	err |= clSetKernelArg(kernelArray, 1, sizeof(cl_mem), (void *)&hDeviceVel);
	err |= clSetKernelArg(kernelArray, 2, sizeof(cl_mem), (void *)&hDeviceHash);

    if (err != CL_SUCCESS)
    {
        printf("Error: Failed to set kernelArray arguments! %d\n", err);
        return false;
    }
	return true;
}

void Fountain::ParticlesInit(int n, Particle *particle)
{
	int i;

	srand(2112);

	Particle *iter;

	float angle = RandRange(-(float)M_PI, (float)M_PI);
	for(iter = particle, i = 0; i < n; i++, iter++)
	{
		float arg = RandRange(-(float)M_PI, (float)M_PI);
		iter->hash.s[0] = cos(arg + angle);
		iter->hash.s[1] = arg;
		iter->hash.s[2] = sin(arg + angle);
		iter->hash.s[3] = RandRange(0.0f, 15.0f);

		iter->pos.s[0] = 0.0f;
		iter->pos.s[1] = 0.0f;
		iter->pos.s[2] = 0.0f;
		iter->pos.s[3] = 0.0f;

		iter->vel.s[0] = iter->hash.s[0];
		iter->vel.s[1] = iter->hash.s[1];
		iter->vel.s[2] = iter->hash.s[2];
		iter->vel.s[3] = 0.0f;//iter->hash.s[3];

		angle += RandRange(-(float)M_PI, (float)M_PI);
	}
}

void Fountain::ParticlesInit(int n, cl_float4 *pos, cl_float4 *vel, cl_float4 *hash)
{
	int i;

	srand(2112);

	float angle = RandRange(-(float)M_PI, (float)M_PI);
	for(i = 0; i < n; i++)
	{
		float arg = RandRange(-(float)M_PI, (float)M_PI);
		hash[i].s[0] = cos(arg + angle);
		hash[i].s[1] = arg;
		hash[i].s[2] = sin(arg + angle);
		hash[i].s[3] = RandRange(0.0f, 15.0f);

		pos[i].s[0] = 0.0f;
		pos[i].s[1] = 0.0f;
		pos[i].s[2] = 0.0f;
		pos[i].s[3] = 0.0f;

		vel[i].s[0] = hash[i].s[0];
		vel[i].s[1] = hash[i].s[1];
		vel[i].s[2] = hash[i].s[2];
		vel[i].s[3] = 0.0f;//hash[i].s[3];

		angle += RandRange(-(float)M_PI, (float)M_PI);
	}
}

bool Fountain::ReleaseCL()
{
	if (strFountainProgram != NULL)
	{
		FreeFileMemory(&strFountainProgram);
	}
	if (hash)
	{
		delete [] hash;
		hash = NULL;
	}
	if (pos)
	{
		delete [] pos;
		pos = NULL;
	}
	if (vel)
	{
		delete [] vel;
		vel = NULL;
	}
	if (particle)
	{
		delete [] particle;
		particle = NULL;
	}
	return CLContext::ReleaseCL();
}

bool Fountain::InitGL()
{
	if (!InitCL())
	{
		return false;
	}

	GLResourceManager &loader = GLResourceManager::Instance();

	// Load font used for overlay rendering
	if (!ttFont.LoadFont("data/fonts/LucidaBrightDemiBold.ttf", 20))
		return false;	

	if (!loader.LoadShaderFromFile("data/shaders/Lookup.vert", "data/shaders/Lookup.frag", uiTextProgram))
		return false;

	if (!loader.LoadShaderFromFile("data/shaders/Particle.vert", "data/shaders/Particle.frag", uiParticleProgram))
		return false;

	if (!loader.LoadShaderFromFile("data/shaders/ColorOffset.vert", "data/shaders/ColorOffset.frag", uiCoordFrame))
		return false;

	if (!fpsGraph.Init(4.0f, 3000, -0.98f, 0.65f, -0.65f, 0.95f, false))
		return false;

	fPointSize = 3.0f;
	fDistance = 2.0f;
	fDefDistance = 10.0f;

	CoordinateFrame::Instance().Make(fDefDistance);

	Resize(ShellGet(SHELL_WIDTH), ShellGet(SHELL_HEIGHT));

	glPointSize(fPointSize);

	x = y = 0.0f;
	camRotate = 0.0f;

	iCurPIDParam = -1;

	pidController.Init(TARGET_FRAMERATE, 1.0f, 0.0f, 0.0f, 1000.0f);

	timer.Start();

	return true;
}


bool Fountain::Resize(unsigned int width, unsigned int height)
{
	//printf("Reshaping Screen to %dx%d\n", width, height);
	// Select and setup the projection matrix
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluPerspective( 65.0f, (GLfloat)width/(GLfloat)height, 0.1f, 400.0f);
	return true;
}

void Fountain::UpdatePID(float dt)
{
	float out = pidController.Update(dt, 1.0f / dt);

	ParticleVBO *vbo = bUseStrideKernel ? pParticleVBOStride : pParticleVBOArray;

	nParticles = vbo->GetCount();

	nParticles -= out;
	if ((nParticles /= 1.02f) < MIN_SIZE)
		nParticles = MIN_SIZE;
	if ((nParticles *= 1.02f) > BUFFERS_SIZE)
		nParticles = BUFFERS_SIZE;

	vbo->SetCount(nParticles);
}


bool Fountain::Input(float t, float dt)
{
	fpsGraph.Input(1.0f / dt, t);

	if (bUsePID)
	{
		if (KeyPressed(KEY_LEFT))
		{
			pidController.UpdateParam(iCurPIDParam, iCurPIDParam == 3 || iCurPIDParam == -1 ? -1.0f : -0.1f);
		}
		if (KeyPressed(KEY_RIGHT))
		{
			pidController.UpdateParam(iCurPIDParam, iCurPIDParam == 3 || iCurPIDParam == -1 ? 1.0f : 0.1f);
		}
		if (KeyPressed(KEY_UP))
		{
			iCurPIDParam = iCurPIDParam > 0 ? iCurPIDParam - 1 : 3;
		}
		if (KeyPressed(KEY_DOWN))
		{
			iCurPIDParam = iCurPIDParam < 3 ? iCurPIDParam + 1 : -1;
		}
		UpdatePID(dt);
	}


	if (KeyPressed(KEY_1))
	{
		bUseStrideKernel = !bUseStrideKernel;
	}
	if (KeyPressed(KEY_2))
	{
		bDisableRendering = !bDisableRendering;
	}
	if (KeyPressed(KEY_3))
	{
		bUsePID = !bUsePID;
	}

	if (ScrollUp())
	{
		fDistance /= 1.25f;
	}
	if (ScrollDown())
	{
		fDistance *= 1.25f;
	}

	/*if (KeyPressing(KEY_LEFT))
	{
		if ((nParticles /= 1.02f) < MIN_SIZE))
			nParticles = MIN_SIZE;

		pParticleVBO->SetCount(nParticles);
	}
	if (KeyPressing(KEY_RIGHT))
	{
		if ((nParticles *= 1.02f) > BUFFERS_SIZE)
			nParticles = BUFFERS_SIZE;

		pParticleVBO->SetCount(nParticles);
	}*/
	if (KeyPressed(KEY_9))
	{
		if ((fPointSize -= 1.0f) < 1.0f)
			fPointSize = 1.0f;

		glPointSize(fPointSize);
	}
	if (KeyPressed(KEY_0))
	{
		if ((fPointSize += 1.0f) > 3.0f)
			fPointSize = 3.0f;

		glPointSize(fPointSize);
	}
	/* Update camera variables */
	y += 0.002f * (GetPointer()->GetMotionX() + GetPointer()->GetInertiaX());
	x -= 0.002f * (GetPointer()->GetMotionY() + GetPointer()->GetInertiaY());
	if (GetPointer()->TimeSinceLastInput() > 10.0f)
		camRotate += timer.GetDeltaTime();

	return true;
}

bool Fountain::RunCL(float t, float dt)
{
	/* Execute OpenCL */
	int err = 0;
	float cldt = dt / 10.0f;
	float g = 10.0f;

	if (bUseStrideKernel)
	{
		Timer temp;

		err |= clSetKernelArg(kernelStride, 1, sizeof(int), &nParticles);
		err |= clSetKernelArg(kernelStride, 2, sizeof(float), &t);
		err |= clSetKernelArg(kernelStride, 3, sizeof(float), &cldt);
		err |= clSetKernelArg(kernelStride, 4, sizeof(float), &g);

		if (err != CL_SUCCESS)
		{
			printf("Error: clSetKernelArg failed!\n");
			return false;
		}

		err |= clEnqueueAcquireGLObjects(GetCommandQueue(), 1, &hDeviceParticle, 0, NULL, NULL);
		if (err != CL_SUCCESS)
		{
			printf("Error: clEnqueueAcquireGLObjects failed!\n");
			return false;
		}

		err = clEnqueueNDRangeKernel(GetCommandQueue(), kernelStride, 1, 0,
			&globalSizeStride, &localSizeStride, 0, 0, 0);

		if (err != CL_SUCCESS)
		{
			printf("Error: clEnqueueNDRangeKernel failed!\n");
			return false;
		}

		//Read(cldt, g);

		err = clEnqueueReleaseGLObjects(GetCommandQueue(), 1, &hDeviceParticle, 0, NULL, NULL);
		clFlush(GetCommandQueue()); 


		fTimeStride = temp.Update();
	}
	else
	{
		Timer temp;

		err |= clSetKernelArg(kernelArray, 3, sizeof(int), &nParticles);
		err |= clSetKernelArg(kernelArray, 4, sizeof(float), &t);
		err |= clSetKernelArg(kernelArray, 5, sizeof(float), &cldt);
		err |= clSetKernelArg(kernelArray, 6, sizeof(float), &g);

		if (err != CL_SUCCESS)
		{
			printf("Error: clSetKernelArg failed!\n");
			return false;
		}

		err |= clEnqueueAcquireGLObjects(GetCommandQueue(), 1, &hDevicePos, 0, NULL, NULL);
		if (err != CL_SUCCESS)
		{
			printf("Error: clEnqueueAcquireGLObjects failed!\n");
			return false;
		}

		err = clEnqueueNDRangeKernel(GetCommandQueue(), kernelArray, 1, 0,
			&globalSizeArray, &localSizeArray, 0, 0, 0);

		if (err != CL_SUCCESS)
		{
			printf("Error: clEnqueueNDRangeKernel failed!\n");
			return false;
		}

		//Read(cldt, g);

		err = clEnqueueReleaseGLObjects(GetCommandQueue(), 1, &hDevicePos, 0, NULL, NULL);
		clFlush(GetCommandQueue()); 

		fTimeArray = temp.Update();
	}
	return true;
}

bool Fountain::RunGL(float t, float dt)
{
	/* GL Rendering */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);

	float yRot = y + 20.0f * camRotate;
	float xRot = x + 20.0f * sin(camRotate);

	if (!bDisableRendering)
	{
		glMatrixMode( GL_MODELVIEW );
		glLoadIdentity();
		glTranslatef(0.0f, -0.5f, -fDistance);
		glRotatef(xRot, 1.0f, 0.0f, 0.0f);
		glRotatef(yRot, 0.0f, 1.0f, 0.0f);

		glEnableClientState(GL_VERTEX_ARRAY);

		// Draw Particle System
		glUseProgram(uiParticleProgram);
		if (bUseStrideKernel)
		{
			pParticleVBOStride->Render(GL_POINTS);
		}
		else
		{
			pParticleVBOArray->Render(GL_POINTS);
		}
	}
	glClear(GL_DEPTH_BUFFER_BIT);

	// Draw Coordinate frame
	DrawCoordinateFrame(xRot, yRot);

	glDisableClientState(GL_VERTEX_ARRAY);


	if (iShowInfo > 0 || bUsePID)
	{
		fpsGraph.Draw();
		
		glUseProgram(uiTextProgram);

		TTFont::glEnable2D();
		
		SDL_Color color;
		SDL_Rect position;

		color.r = 255;
		color.g = 255;
		color.b = 255;

		position.x = (int)(ShellGet(SHELL_WIDTH) * 0.01f);
		position.y = (int)(ShellGet(SHELL_HEIGHT) * 0.75f);

		char text[200];

		if (iShowInfo == 1)
		{
			sprintf(text, "%.1f",  1.0f / dt);
			ttFont.SDL_GL_RenderText(text, color, &position);
		}
		else if (iShowInfo == 2)
		{
			sprintf(text, "FPS=%.1f",  1.0f / dt);
			ttFont.SDL_GL_RenderText(text, color, &position);
			position.y -= position.h * 1.2f;
			sprintf(text, "ms=%.1f",  dt * 1000.0f);
			ttFont.SDL_GL_RenderText(text, color, &position);
			position.y -= position.h * 1.2f;

			sprintf(text, "Particles=%d", nParticles);
			ttFont.SDL_GL_RenderText(text, color, &position);
			position.y -= position.h * 1.2f;

			sprintf(text, "PointSize=%.0f", fPointSize);
			ttFont.SDL_GL_RenderText(text, color, &position);
			position.y -= position.h * 1.2f;

			sprintf(text, "TimeArray=%.3f", fTimeArray);
			ttFont.SDL_GL_RenderText(text, color, &position);
			position.y -= position.h * 1.2f;

			sprintf(text, "TimeStride=%.3f", fTimeStride);
			ttFont.SDL_GL_RenderText(text, color, &position);
			position.y -= position.h * 1.2f;
		}

		if (bUsePID)
		{
			position.y = (int)(ShellGet(SHELL_HEIGHT) * 0.4f);

			sprintf(text, "%starget=%.1f", iCurPIDParam == -1 ? "> " : "", pidController.fTarget);
			ttFont.SDL_GL_RenderText(text, color, &position);
			position.y -= position.h * 1.2f;

			sprintf(text, "%sKp=%.1f", iCurPIDParam == 0 ? "> " : "", pidController.Kp);
			ttFont.SDL_GL_RenderText(text, color, &position);
			position.y -= position.h * 1.2f;

			sprintf(text, "%sKi=%.1f", iCurPIDParam == 1 ? "> " : "", pidController.Ki);
			ttFont.SDL_GL_RenderText(text, color, &position);
			position.y -= position.h * 1.2f;

			sprintf(text, "%sKd=%.1f", iCurPIDParam == 2 ? "> " : "", pidController.Kd);
			ttFont.SDL_GL_RenderText(text, color, &position);
			position.y -= position.h * 1.2f;

			sprintf(text, "%sKg=%.1f", iCurPIDParam == 3 ? "> " : "", pidController.Kg);
			ttFont.SDL_GL_RenderText(text, color, &position);
			position.y -= position.h * 1.2f;

		}
		TTFont::glDisable2D();
		glDisable(GL_BLEND);
	}
	return true;
}

bool Fountain::Render()
{
	/* Input - timer and fps graph */
	timer.Update();
	float t = timer.GetTime();
	float dt = timer.GetDeltaTime();

	return
		Input(t, dt) &&
		RunCL(t, dt) &&
		RunGL(t, dt);
}

bool Fountain::Read(float dt, float g)
{
	int err = 0;

	cl_event events[2];

	err |= clEnqueueReadBuffer(GetCommandQueue(), hDevicePos, CL_TRUE, 0,
		globalSizeArray, pos, 0, 0, &events[0]);

	err |= clEnqueueReadBuffer(GetCommandQueue(), hDeviceVel, CL_TRUE, 0,
		globalSizeArray, vel, 0, 0, &events[1]);

	err = clWaitForEvents(2, &events[0]);
	if (err != CL_SUCCESS)
	{
		printf("Error: clWaitForEvents failed!\n");
		return false;
	}
	clReleaseEvent(events[0]);
	clReleaseEvent(events[1]);

	// Accumulate min,max
	min = pos[0];
	max = pos[0];

	unsigned int i, j;
	for (i = 1; i < nParticles; i++)
	{
		for (j = 0; j < 4; j++)
		{
			if (pos[i].s[j] < min.s[j])
				min.s[j] = pos[i].s[j];
			if (pos[i].s[j] > max.s[j])
				max.s[j] = pos[i].s[j];
		}
	}

	printf("dt = %.6f, g = %.3f\n", dt, g);
	printf("POS = [%.6f, %.6f, %.6f, %.6f]\n",
		pos[0].s[0], pos[0].s[1], pos[0].s[2], pos[0].s[3]);
	printf("VEL = [%.6f, %.6f, %.6f, %.6f]\n",
		vel[0].s[0], vel[0].s[1], vel[0].s[2], vel[0].s[3]);
	printf("min = [%.6f, %.6f, %.6f, %.6f]\n",
		min.s[0], min.s[1], min.s[2], min.s[3]);
	printf("max = [%.6f, %.6f, %.6f, %.6f]\n",
		max.s[0], max.s[1], max.s[2], max.s[3]);

	return true;
}


void Fountain::DrawCoordinateFrame(float xRot, float yRot)
{
	glUseProgram(uiCoordFrame);
	float offset[] = { -0.9f, -0.9f };
	glUniform2fv(GetUniLoc(uiCoordFrame, "Offset"), 1, offset);

	glLoadIdentity();

	glTranslatef(0.0f, 0.0f, -fDefDistance);
	glRotatef(xRot, 1.0f, 0.0f, 0.0f);
	glRotatef(yRot, 0.0f, 1.0f, 0.0f);

	CoordinateFrame::Instance().Render();
}

bool Fountain::ReleaseGL()
{
	ReleaseCL();

	CoordinateFrame::Instance().Unload();

	return GLResourceManager::Instance().Release();
}

bool Fountain::ReleaseApp()
{
	return true;
}

Shell *NewDemo()
{
	return new Fountain();
}



/* Presets */

/*
	// This preset creates a 360 degrees symmetrical water fountain
	// With several levels
	float angle = RandRange(-(float)M_PI, (float)M_PI);
	for(i = 0; i < n; i++)
	{
		float arg = RandRange(-(float)M_PI, (float)M_PI);
		hash[i].s[0] = cos(arg + angle);
		hash[i].s[1] = arg;
		hash[i].s[2] = sin(arg + angle);
		hash[i].s[3] = RandRange(0.0f, 15.0f);

		pos[i].s[0] = 0.0f;
		pos[i].s[1] = 0.0f;
		pos[i].s[2] = 0.0f;
		pos[i].s[3] = 0.0f;

		vel[i].s[0] = hash[i].s[0];
		vel[i].s[1] = hash[i].s[1];
		vel[i].s[2] = hash[i].s[2];
		vel[i].s[3] = 0.0f;//hash[i].s[3];

		angle += RandRange(-(float)M_PI, (float)M_PI);
	}

	// This preset creates a spiral-type water fountain
	// With several levels
	float angle = RandRange(-(float)M_PI, (float)M_PI);
	for(i = 0; i < n; i++)
	{
		float arg = RandRange(-(float)M_PI, (float)M_PI);
		hash[i].s[0] = cos(arg);
		hash[i].s[1] = arg;
		hash[i].s[2] = sin(arg);
		hash[i].s[3] = RandRange(0.0f, 15.0f);

		pos[i].s[0] = 0.0f;
		pos[i].s[1] = 0.0f;
		pos[i].s[2] = 0.0f;
		pos[i].s[3] = 0.0f;

		vel[i].s[0] = hash[i].s[0];
		vel[i].s[1] = hash[i].s[1];
		vel[i].s[2] = hash[i].s[2];
		vel[i].s[3] = 0.0f;//hash[i].s[3];

		angle += RandRange(-(float)M_PI, (float)M_PI);
	}

	// This preset creates a one-axis symmetrical water fountain
	// With several levels
	float angle = RandRange(-(float)M_PI, (float)M_PI);
	for(i = 0; i < n; i++)
	{
		float arg = RandRange(-(float)M_PI, (float)M_PI);
		hash[i].s[0] = cos(arg + angle);
		hash[i].s[1] = arg;
		hash[i].s[2] = sin(arg);
		hash[i].s[3] = RandRange(0.0f, 15.0f);

		pos[i].s[0] = 0.0f;
		pos[i].s[1] = 0.0f;
		pos[i].s[2] = 0.0f;
		pos[i].s[3] = 0.0f;

		vel[i].s[0] = hash[i].s[0];
		vel[i].s[1] = hash[i].s[1];
		vel[i].s[2] = hash[i].s[2];
		vel[i].s[3] = 0.0f;//hash[i].s[3];

		angle += RandRange(-(float)M_PI, (float)M_PI);
	}
	
	// General preset template
	float angle = RandRange(-(float)M_PI, (float)M_PI);
	for(i = 0; i < n; i++)
	{
		float arg = RandRange(-(float)M_PI, (float)M_PI);
		hash[i].s[0] = cos(arg + angle);
		hash[i].s[1] = arg;
		hash[i].s[2] = sin(arg + angle);
		hash[i].s[3] = RandRange(0.0f, 15.0f);

		pos[i].s[0] = 0.0f;//RandRange(-0.2f, 0.2f);
		pos[i].s[1] = 0.0f;//RandRange(0.0f, 2.0f);
		pos[i].s[2] = 0.0f;//RandRange(-0.2f, 0.2f);
		pos[i].s[3] = 0.0f;

		vel[i].s[0] = 0.1 * cos(angle);
		vel[i].s[1] = RandRange(0.3f, 1.8f);
		vel[i].s[2] = 0.1 * sin(angle);
		vel[i].s[3] = 0.0f;

		vel[i].s[0] = hash[i].s[0];
		vel[i].s[1] = hash[i].s[1];
		vel[i].s[2] = hash[i].s[2];
		vel[i].s[3] = 0.0f;//hash[i].s[3];

		angle += RandRange(-(float)M_PI, (float)M_PI);
	}
*/

