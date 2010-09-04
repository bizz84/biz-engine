/*****************************************************************************
 * Filename			Fountain.cpp
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

#include "Fountain.h"
#include "TextGraph.h"
#include "Fonts/FontTechno.h"

#include <CL/cl.hpp>

const char *aszModes[] = {
	"OPENCL ARRAYS",
	"OPENCL STRUCT", 
	"CPU REFERENCE"
};

#define BUFFERS_SIZE (1 << 20)
#define MIN_SIZE (1 << 8)

#define TARGET_FRAMERATE 30.0f
#define TARGET_FRAMETIME (1.0f / TARGET_FRAMERATE)

Fountain::Fountain() :
	pos(NULL),
	vel(NULL),
	hash(NULL),
	strFountainProgram(NULL),
	pParticleVBOArray(NULL),
	pParticleVBOStride(NULL),

	bModeChanged(false)
{

}

Fountain::~Fountain()
{

}

bool Fountain::InitApp()
{
	// Standard initialization
	bDisableRendering = false;

	eMode = OPENCL_ARRAY;

	sImpl[CPU].nParticles = BUFFERS_SIZE;


	// Initialization for command line modifiable variables
	iShowInfo = 0;
	bUsePID = false;

	// Custom command line processing
	vector<CmdLineParameter>::const_iterator iter;
	const vector<CmdLineParameter> &params = GetCommandLineParams();

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

bool Fountain::SetupArrayKernel()
{
	int err;
	/* ------------------------  Array version  -------------------------------- */
	sImpl[OPENCL_ARRAY].nParticles = BUFFERS_SIZE;
	
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

bool Fountain::SetupStrideKernel()
{
	int err;
	/* ------------------------  Stride version  -------------------------------- */
	sImpl[OPENCL_STRUCT].nParticles = BUFFERS_SIZE;

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
		iter->hash.s[1] = 3.0f + arg;
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
		hash[i].s[1] = 3.0f + arg;
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

	if (!loader.LoadShaderFromFile("data/shaders/Lookup.vert", "data/shaders/Lookup.frag", uiTextProgram))
		return false;

	if (!loader.LoadShaderFromFile("data/shaders/Particle.vert", "data/shaders/Particle.frag", uiParticleProgram))
		return false;

	if (!loader.LoadShaderFromFile("data/shaders/ColorOffset.vert", "data/shaders/ColorOffset.frag", uiCoordFrame))
		return false;

	fPointSize = 3.0f;
	fDistance = 2.0f;
	fDefDistance = 10.0f;

	CoordinateFrame::Instance().Make(fDefDistance);


	pFont = auto_ptr<FontManager>(new FontTechno("data/textures/FontB.bmp"));
	if (!pFont->Init())
		return false;

	// Initialize FPS graph
	pFPSGraph = auto_ptr<TextGraph>(new TextGraph());
	//if (!pFPSGraph->Init(true, true, true, true, "%.1f", 0.075f, 4.0f, 3000, 0.6f, 0.55f, 0.98f, 0.90f, false))
	if (!pFPSGraph->Init(true, true, true, true, "%.1f", 0.08f, 4.0f, 3000, 0.35f, 0.55f, 0.98f, 0.90f, false))
		return false;

	pPartGraph = auto_ptr<TextGraph>(new TextGraph());
	if (!pPartGraph->Init(true, true, true, true, "%.0f", 0.08f, 4.0f, 3000, 0.35f, -0.1f, 0.98f, 0.35f, false))
		return false;


	Resize(ShellGet(SHELL_WIDTH), ShellGet(SHELL_HEIGHT));

	glPointSize(fPointSize);

	x = y = 0.0f;
	camRotate = 0.0f;

	for (unsigned int i = 0; i < NUM_IMPLEMENTATIONS; i++)
	{
		sImpl[i].bFullSteam = bUsePID;
		sImpl[i].nParticles = bUsePID ? BUFFERS_SIZE : MIN_SIZE;
	}
	if (!bUsePID)
	{
		pParticleVBOArray->SetCount(MIN_SIZE);
		pParticleVBOStride->SetCount(MIN_SIZE);
	}

	iCurPIDParam = -1;

	pidController.Init(TARGET_FRAMETIME, 1.0f, 0.0f, 0.0f, 200000.0f);


	glEnable(GL_TEXTURE_2D);
	glEnableClientState(GL_VERTEX_ARRAY);	
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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

void Fountain::CPUReference(float g, float dt)
{
	unsigned int i;
	Particle *iter;
	for (iter = particle, i = 0; i < sImpl[CPU].nParticles; i++, iter++)
	{
		float f = g * dt;

		iter->vel.s[1] -= f;
		
		iter->pos.s[0] += iter->vel.s[0] * dt;
		iter->pos.s[1] += iter->vel.s[1] * dt;
		iter->pos.s[2] += iter->vel.s[2] * dt;
		iter->pos.s[3] += dt;
		
		if (iter->pos.s[1] < 0.0f)
		{
			iter->vel.s[3] += 1.0f;
			if (iter->vel.s[3] > iter->hash.s[3])
			{
				iter->vel = iter->hash;
				iter->vel.s[3] = 0.0;

				iter->pos.s[0] = 0.0f;
				iter->pos.s[1] = 0.0f;
				iter->pos.s[2] = 0.0f;
				iter->pos.s[3] = 0.0f;
			}
			else
			{
				iter->pos.s[1] = -iter->pos.s[1];
				iter->vel.s[1] *= -0.5f;
			}
		}
	}
}

void Fountain::UpdatePID(float dt, unsigned int &nParticles)
{
	float out = pidController.Update(dt, dt);

	nParticles += out;
	if ((nParticles /= 1.02f) < MIN_SIZE)
		nParticles = MIN_SIZE;
	if ((nParticles *= 1.02f) > BUFFERS_SIZE)
		nParticles = BUFFERS_SIZE;

	if (eMode != CPU)
	{
		ParticleVBO *vbo = eMode == OPENCL_STRUCT ? pParticleVBOStride : pParticleVBOArray;
		vbo->SetCount(nParticles);
	}
}


bool Fountain::Input(float t, float dt)
{
	pFPSGraph->Input(1.0f / dt, t);

	if ((bModeChanged = KeyPressed(KEY_1)))
	{
		eMode = Next(eMode, NUM_IMPLEMENTATIONS);
	}
	if (KeyPressed(KEY_2))
	{
		bUsePID = !bUsePID;
	}
	if (KeyPressed(KEY_3))
	{
		bDisableRendering = !bDisableRendering;
	}
	// Set reference # of particles for all implementations
	if (KeyPressed(KEY_4))
	{
		unsigned int i, nParticles = sImpl[eMode].nParticles;
		for (i = 0; i < NUM_IMPLEMENTATIONS; i++)
		{
			sImpl[i].nParticles = nParticles;
			sImpl[i].bFullSteam = true;
		}
		pParticleVBOArray->SetCount(nParticles);
		pParticleVBOStride->SetCount(nParticles);
	}

	if (bUsePID)
	{
		if (KeyPressed(KEY_LEFT))
		{
			// Convert to fps and back
			if (iCurPIDParam == -1)
			{
				float f = 1.0f / pidController.fTarget - 1.0f;
				pidController.fTarget = 1.0f / f;
			}
			else
				pidController.UpdateParam(iCurPIDParam, iCurPIDParam == 3 ? -1.0f : -0.1f);
		}
		if (KeyPressed(KEY_RIGHT))
		{
			if (iCurPIDParam == -1)
			{
				float f = 1.0f / pidController.fTarget + 1.0f;
				pidController.fTarget = 1.0f / f;
			}
			else
				pidController.UpdateParam(iCurPIDParam, iCurPIDParam == 3 ? 1.0f : 0.1f);
		}
		if (KeyPressed(KEY_UP))
		{
			iCurPIDParam = iCurPIDParam > -1 ? iCurPIDParam - 1 : 3;
		}
		if (KeyPressed(KEY_DOWN))
		{
			iCurPIDParam = iCurPIDParam < 3 ? iCurPIDParam + 1 : -1;
		}
		if (!bModeChanged)
			UpdatePID(dt, sImpl[eMode].nParticles);
		pPartGraph->Input((float)sImpl[eMode].nParticles, t);
	}
	else
	{
		ParticleVBO *vbo = NULL;
		if (eMode == OPENCL_ARRAY)
			vbo = pParticleVBOArray;
		else if (eMode == OPENCL_STRUCT)
			vbo = pParticleVBOStride;

		unsigned int &nParticles = sImpl[eMode].nParticles;
		bool &bFullSteam = sImpl[eMode].bFullSteam;

		if (!sImpl[eMode].bFullSteam)
		{
			if ((nParticles *= 1.0f + dt) > BUFFERS_SIZE)
			{
				bFullSteam = true;
				nParticles = BUFFERS_SIZE;
			}
			if (vbo)
				vbo->SetCount(sImpl[eMode].nParticles);
		}
		if (KeyPressing(KEY_LEFT))
		{
			if (!bFullSteam)
				bFullSteam = true;

			if ((nParticles /= 1.02f) < MIN_SIZE)
				nParticles = MIN_SIZE;

			if (vbo)
				vbo->SetCount(sImpl[eMode].nParticles);
		}
		if (KeyPressing(KEY_RIGHT))
		{
			if (!bFullSteam)
				bFullSteam = true;

			if ((nParticles *= 1.02f) > BUFFERS_SIZE)
				nParticles = BUFFERS_SIZE;

			if (vbo)
				vbo->SetCount(nParticles);
		}
	}




	if (ScrollUp())
	{
		fDistance /= 1.25f;
	}
	if (ScrollDown())
	{
		fDistance *= 1.25f;
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

	Timer temp;
	switch (eMode)
	{
	case OPENCL_STRUCT:
		err |= clSetKernelArg(kernelStride, 1, sizeof(int), &sImpl[eMode].nParticles);
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
		break;
	case OPENCL_ARRAY:
		err |= clSetKernelArg(kernelArray, 3, sizeof(int), &sImpl[eMode].nParticles);
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
			&globalSizeArray, NULL/*&localSizeArray*/, 0, 0, 0);

		if (err != CL_SUCCESS)
		{
			printf("Error: clEnqueueNDRangeKernel failed!\n");
			return false;
		}

		//Read(cldt, g);

		err = clEnqueueReleaseGLObjects(GetCommandQueue(), 1, &hDevicePos, 0, NULL, NULL);
		clFlush(GetCommandQueue()); 
		break;
	case CPU:
		CPUReference(g, dt);
		break;
	}
	sImpl[eMode].fRunTime = temp.Update();

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

		// Draw Particle System
		glUseProgram(uiParticleProgram);
		if (eMode == OPENCL_STRUCT)
		{
			pParticleVBOStride->Render(GL_POINTS);
		}
		else if (eMode == OPENCL_ARRAY)
		{
			pParticleVBOArray->Render(GL_POINTS);
		}
		else
		{
			//CPU
			glBindBuffer(GL_ARRAY_BUFFER_ARB, 0);
			glVertexPointer(4, GL_FLOAT, sizeof(Particle), particle);
			glDrawArrays(GL_POINTS, 0, sImpl[eMode].nParticles);
		}
	}
	glClear(GL_DEPTH_BUFFER_BIT);

	// Draw Coordinate frame
	DrawCoordinateFrame(xRot, yRot);


	if (iShowInfo > 0 || bUsePID)
	{
		pFPSGraph->Draw();
		if (bUsePID)
			pPartGraph->Draw();

		glEnable(GL_BLEND);
		pFont->Bind();

		pFPSGraph->TextDraw(pFont.get());
		if (bUsePID)
			pPartGraph->TextDraw(pFont.get());

		// font parameters
		float yellow[] = {1.0,1.0,0.0,1.0};
		float red[] = {1.0,0.0,0.0,1.0};
		float scale = 0.06f;
		float mscale = 0.07f;
		float x = -1.0f;
		float y = 1.0f;

		FontManager::HorzAlign horz = FontManager::LeftAlign;
		FontManager::VertAlign vert = FontManager::TopAlign;

		if (iShowInfo >= 2)
		{
			pFont->Render(x, y, scale * 1.5f, red, horz, vert,
				aszModes[eMode]);

			y -= mscale * 1.5f;

			/*pFont->Render(x, y -= mscale, scale, yellow, horz, vert,
				"ms=%.1f",  dt * 1000.0f);*/

			if (!bUsePID)
				pFont->Render(x, y -= mscale, scale, yellow, horz, vert,
					"Particles=%d", sImpl[eMode].nParticles);

			pFont->Render(x, y -= mscale, scale, yellow, horz, vert,
				"PointSize=%.0f", fPointSize);

			pFont->Render(x, y -= mscale, scale, yellow, horz, vert,
				"RunTime=%.3f", sImpl[eMode].fRunTime);
		}
		if (bUsePID)
		{
			y -= mscale;

			float green[] = {0.0,1.0,0.0,1.0};
			pFont->Render(x, y -= mscale, scale, iCurPIDParam == -1 ? green : yellow, horz, vert,
				"target=%.1f", 1.0f / pidController.fTarget);

			pFont->Render(x, y -= mscale, scale, iCurPIDParam == 0 ? green : yellow, horz, vert,
				"Kp=%.1f", pidController.Kp);

			pFont->Render(x, y -= mscale, scale, iCurPIDParam == 1 ? green : yellow, horz, vert,
				"Ki=%.1f", pidController.Ki);

			pFont->Render(x, y -= mscale, scale, iCurPIDParam == 2 ? green : yellow, horz, vert,
				"Kd=%.1f", pidController.Kd);

			pFont->Render(x, y -= mscale, scale, iCurPIDParam == 3 ? green : yellow, horz, vert,
				"Kg=%.1f", pidController.Kg);
		}		
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
	for (i = 1; i < sImpl[eMode].nParticles; i++)
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

