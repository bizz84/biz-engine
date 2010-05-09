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

Fountain::Fountain()
{
	pos = vel = NULL;
	hash = NULL;
	strFountainKernel = NULL;
}

Fountain::~Fountain()
{

}

bool Fountain::InitApp()
{
	iShowInfo = 0;

	// Custom command line processing
	vector<CmdLineParameter>::iterator iter;
	vector<CmdLineParameter> &params = GetCommandLineParams();

	for (iter = params.begin(); iter != params.end(); iter++)
	{
		if (iter->sName == "showinfo")
		{
			iShowInfo = atoi(iter->sValue.c_str());
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

	// Load kernel from file
	if (LoadFileIntoMemory("data/kernels/fountain.cl", &strFountainKernel) < 0)
		return false;

	// Create the compute program from the source buffer
    program = clCreateProgramWithSource(context, 1, (const char **) & strFountainKernel, NULL, &err);
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

    // Create the compute kernel in the program we wish to run
    kernel = clCreateKernel(program, "fountain_kern", &err);
    if (!kernel || err != CL_SUCCESS)
    {
        printf("Error: Failed to create compute kernel!\n");
        return false;
    }

	// Get work group size for kernel
	err = clGetKernelWorkGroupInfo(kernel, device_id, CL_KERNEL_WORK_GROUP_SIZE, sizeof(workGroupSize), &workGroupSize, NULL);
    if (err != CL_SUCCESS)
    {
        printf("Error: Failed to retrieve kernel work group info! %d\n", err);
        return false;
    }
    localGroupSize = workGroupSize / sizeof(cl_float4);

	// Create host buffers 

	// Instance client vectors
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

	pParticleVBO = new ParticleVBO(pos, BUFFERS_SIZE);

	hDevicePos = clCreateFromGLBuffer(GetContext(),
		CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,//CL_MEM_ALLOC_HOST_PTR,
		pParticleVBO->GetVBO(), &err);


	// Check for correctness
	if (!hDevicePos || !hDeviceVel)
    {
        printf("Error: Failed to create buffers!\n");
		return false;
	}
	// Set Parameters

	err = 0;
	// set attributes
	err |= clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&hDevicePos);
	err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&hDeviceVel);
	err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&hDeviceHash);

    if (err != CL_SUCCESS)
    {
        printf("Error: Failed to set kernel arguments! %d\n", err);
        return false;
    }

	// Other initialization
	localSize = localGroupSize * sizeof(cl_float4);
	globalSize = BUFFERS_SIZE * sizeof(cl_float4);

	
	return true;
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
	if (strFountainKernel != NULL)
	{
		FreeFileMemory(&strFountainKernel);
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

bool Fountain::Input(float t, float dt)
{
	fpsGraph.Input(1.0f / dt, t);

	if (ScrollUp())
	{
		fDistance /= 1.25f;
	}
	if (ScrollDown())
	{
		fDistance *= 1.25f;
	}

	if (KeyPressing(KEY_LEFT))
	{
		if ((nParticles /= 1.02f) < (1 << 10))
			nParticles = (1 << 10);

		pParticleVBO->SetCount(nParticles);
	}
	if (KeyPressing(KEY_RIGHT))
	{
		if ((nParticles *= 1.02f) > BUFFERS_SIZE)
			nParticles = BUFFERS_SIZE;

		pParticleVBO->SetCount(nParticles);
	}
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

	err |= clSetKernelArg(kernel, 3, sizeof(int), &nParticles);
	err |= clSetKernelArg(kernel, 4, sizeof(float), &t);
	err |= clSetKernelArg(kernel, 5, sizeof(float), &cldt);
	err |= clSetKernelArg(kernel, 6, sizeof(float), &g);

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

	err = clEnqueueNDRangeKernel(GetCommandQueue(), kernel, 1, 0,
		&globalSize, &localSize, 0, 0, 0);

	if (err != CL_SUCCESS)
	{
		printf("Error: clEnqueueNDRangeKernel failed!\n");
		return false;
	}

	//Read(cldt, g);

	err = clEnqueueReleaseGLObjects(GetCommandQueue(), 1, &hDevicePos, 0, NULL, NULL);
	clFlush(GetCommandQueue()); 

	return true;
}

bool Fountain::RunGL(float t, float dt)
{
	/* GL Rendering */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);

	float yRot = y + 20.0f * camRotate;
	float xRot = x + 20.0f * sin(camRotate);

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	glTranslatef(0.0f, -0.5f, -fDistance);
	glRotatef(xRot, 1.0f, 0.0f, 0.0f);
	glRotatef(yRot, 0.0f, 1.0f, 0.0f);

	glEnableClientState(GL_VERTEX_ARRAY);

	// Draw Particle System
	glUseProgram(uiParticleProgram);
	pParticleVBO->Render(GL_POINTS);

	glClear(GL_DEPTH_BUFFER_BIT);

	// Draw Coordinate frame
	DrawCoordinateFrame(xRot, yRot);

	glDisableClientState(GL_VERTEX_ARRAY);


	if (iShowInfo > 0)
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

		if (iShowInfo == 1)
		{
			char text[200];
			sprintf(text, "%.1f",  1.0f / timer.GetDeltaTime());
			ttFont.SDL_GL_RenderText(text, color, &position);
		}
		else if (iShowInfo == 2)
		{
			char text[200];
			sprintf(text, "FPS=%.1f",  1.0f / timer.GetDeltaTime());
			ttFont.SDL_GL_RenderText(text, color, &position);
			position.y -= position.h * 1.2f;

			sprintf(text, "Particles=%d", nParticles);
			ttFont.SDL_GL_RenderText(text, color, &position);
			position.y -= position.h * 1.2f;

			sprintf(text, "PointSize=%.0f", fPointSize);
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
		globalSize, pos, 0, 0, &events[0]);

	err |= clEnqueueReadBuffer(GetCommandQueue(), hDeviceVel, CL_TRUE, 0,
		globalSize, vel, 0, 0, &events[1]);

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

