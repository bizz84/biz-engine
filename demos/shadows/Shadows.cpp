/*****************************************************************************
 * Filename			Shadows.cpp
 * 
 * License			LGPL
 *
 * Author			Andrea Bizzotto (bizz84@gmail.com)
 *
 * Platform			LinuxX11 / OpenGL
 * 
 * Description		Lighting & Shadows demo using stencil buffers
 *					This sample implementation uses the Z-fail algorithm
 *					(aka Carmack's reverse)
 *
 *****************************************************************************/

#include "Shadows.h"

enum DisplayMode {
	E_SHADOWS,
	E_SHADOW_VOLUMES,
	E_STENCIL,
	//E_NO_SHADOWS, 
	E_DISPLAY_MODES 
};

const char *DisplayMoreStr[] = {
	"Shadow Volumes",
	"Shadows",
	"Stencil Count",
	"No Shadows"
};

const float TetraVertices[12] = {
	 0.0f    ,  1.0f    ,  0.0     , 
	 0.0f    , -0.33333f,  0.94281f,
	-0.81650f, -0.33333f, -0.47140f,
	 0.81650f, -0.33333f, -0.47140f
};

const int TetraIndices[12] = {
	0, 1, 2,
	0, 2, 3,
	0, 3, 1,
	1, 2, 3
};

const float Colors[] = {

	0.0f, 1.0f, 0.0f, 0.8f,
	0.0f, 1.0f, 0.0f, 0.6f,
	0.0f, 1.0f, 0.0f, 0.4f,
	0.0f, 1.0f, 0.0f, 0.2f,
	1.0f, 1.0f, 0.0f, 0.2f,
};

const float GrayColor[] = { 0.5f, 0.5f, 0.5f, 1.0f };
const float WhiteColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };



Shadows::Shadows()
{
	t = x = y = 0.0f;
	bWireframe = false;
	unsigned int i;
	for (i = 0; i < MESH_NUM; i++)
	{
		pSVMesh[i] = NULL;
	}
	pTetraVBO = NULL;
	pRoomMesh = NULL;

	eDisplayMode = E_SHADOWS;

	iFrameCounter = 0;

	iCurrentGroup = 0;

	iCurLight = 0;
	iNumLights = 1;
	light[0].fLightSpeed = 0.5f;
	light[0].fLightAngle = 0.0f;
	light[0].fLightHeight = 100.0f;
	light[0].fLightRadius = 1.0f;

	light[1].fLightSpeed = 0.6f;
	light[1].fLightAngle = 0.5 * M_PI;
	light[1].fLightHeight = -100.0f;
	light[1].fLightRadius = 1.2f;

	light[2].fLightSpeed = 0.35f;
	light[2].fLightAngle = M_PI;
	light[2].fLightHeight = 50.0f;
	light[2].fLightRadius = 1.5f;

	light[3].fLightSpeed = 0.4f;
	light[3].fLightAngle = M_PI + M_PI_2;
	light[3].fLightHeight = 40.0f;
	light[3].fLightRadius = 1.8f;


	bLightMove = true;

	bUseDoubleStencil = true;
}

bool Shadows::InitApp()
{
	iShowInfo = 0;
	bFPSMode = false;

	// Custom command line processing
	vector<CmdLineParameter>::iterator iter;
	vector<CmdLineParameter> &params = GetCommandLineParams();

	for (iter = params.begin(); iter != params.end(); iter++)
	{
		if (iter->sName == "showinfo")
		{
			iShowInfo = atoi(iter->sValue.c_str());
		}
		if (iter->sName == "fpsmode")
		{
			bFPSMode = (bool)atoi(iter->sValue.c_str());
		}
		
	}	

	return true;
}

bool Shadows::LoadShaders()
{
	GLResourceManager &loader = GLResourceManager::Instance();
	for (unsigned int i = 0; i < E_NUM_PROGRAMS; i++)
	{
		if (!loader.LoadShaderFromFile(shaders[i*2+0], shaders[i*2+1],
			uiProgram[i]))
			return false;
	}
	return true;
}

bool Shadows::InitGL()
{
	GLResourceManager &loader = GLResourceManager::Instance();

	// Load Shaders
	if (!LoadShaders())
		return false;

	// Load font used for overlay rendering
	if (!ttFont.LoadFont("data/fonts/LucidaBrightDemiBold.ttf", 20))
		return false;	

	if (!loader.LoadTextureFromFile("data/textures/crate-base.bmp",
		uiBGTexture, GL_LINEAR, GL_LINEAR))
		return false;
	
	unsigned int handle3DSFile[2];
	if (!loader.Load3DSFile("data/scenes/primitives.3ds", handle3DSFile[0]))
		return false;

	if (!loader.Load3DSFile("data/scenes/sphere-cube.3ds", handle3DSFile[1]))
		return false;

	//if (!loader.Load3DSFile("data/scenes/monkey.3ds", handle3DSFile[2]))
	//	return false;

	// Load outer room
	pRoomMesh = new Mesh(loader.FindMesh(handle3DSFile[0], "Box"));
	
	// Load Object Meshes and generate Shadow Volumes
	pSVMesh[MESH_TORUS] = new ShadowVolumeMesh(
		loader.FindMesh(handle3DSFile[0], meshes[MESH_TORUS]));
	pSVMesh[MESH_TORUS_KNOT] = new ShadowVolumeMesh(
		loader.FindMesh(handle3DSFile[0], meshes[MESH_TORUS_KNOT]));
	
	pSVMesh[MESH_CUBE] = new ShadowVolumeMesh(
		loader.FindMesh(handle3DSFile[1], meshes[MESH_CUBE]));
	pSVMesh[MESH_SPHERE] = new ShadowVolumeMesh(
		loader.FindMesh(handle3DSFile[1], meshes[MESH_SPHERE]));

	//pSVMesh[MESH_MONKEY] = new ShadowVolumeMesh(
	//	loader.FindMesh(handle3DSFile[2], meshes[MESH_MONKEY]), 60.0f);

	aaiMeshGroup[0].push_back(MESH_TORUS_KNOT);
	aaiMeshGroup[1].push_back(MESH_TORUS);
	aaiMeshGroup[2].push_back(MESH_SPHERE);
	aaiMeshGroup[2].push_back(MESH_CUBE);


	CalculateBoundaries(handle3DSFile[0]);

	fMinShadowExtent = 0.0f;
	fMaxShadowExtent = 2.0f * fMaxDistance;
	fShadowExtent = fMaxShadowExtent;

	pTetraVBO = new IndexedVBO((void *)TetraVertices, sizeof(float) * 3, 4,
	                           (void *)TetraIndices, 12);


	if (!fpsGraph.Init(4.0f, 3000, -0.98f, 0.60f, -0.6f, 0.95f, false))
	{
		return false;
	}


	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );

	Resize(ShellGet(SHELL_WIDTH), ShellGet(SHELL_HEIGHT));

	glDisable(GL_DEPTH_TEST);

	// Hack
	fMaxDistance *= 3.0f;

	fpsCamera.Init(fDefDistance, -20.0f);

	timer.Start();
	return true;
}

void Shadows::CalculateBoundaries(int index3ds)
{
	GLResourceManager &loader = GLResourceManager::Instance();

	// Calculating maximum and minimum range for zoom
	fBoxSize = loader.MeshSize(index3ds, "Box");

	fMaxDistance = 0.5f * fBoxSize;
	
	float d = fMinDistance = loader.MeshSize(index3ds, "Torus");
	d = loader.MeshSize(index3ds, "TorusKnot");
	if (d > fMinDistance)
		fMinDistance = d;
	d = loader.MeshSize(index3ds, "Teapot");
	if (d > fMinDistance)
		fMinDistance = d;

	fDistance = fDefDistance = 0.5f * (fMaxDistance + fMinDistance);

	CoordinateFrame::Instance().Make(fMaxDistance);
}

bool Shadows::Resize(unsigned int width, unsigned int height)
{
	//printf("Reshaping Screen to %dx%d\n", width, height);
	// Select and setup the projection matrix
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluPerspective( 65.0f, (GLfloat)width/(GLfloat)height, 10.0f, 4000.0f);
	glMatrixMode( GL_MODELVIEW );
	return true;
}

void Shadows::UpdateModelView()
{
	// Select and setup the modelview matrix
	glMatrixMode( GL_MODELVIEW );

	if (bFPSMode)
	{
		fpsCamera.LoadMatrix();
	}
	else
	{
		//spinCamera.LoadMatrix();
		glLoadIdentity();

		glTranslatef(0.0f, 0.0f, -fDistance);
		glRotatef(xRot, 1.0f, 0.0f, 0.0f);
		glRotatef(yRot, 0.0f, 1.0f, 0.0f);
	}
}


bool Shadows::Render()
{
	/* Input - timer and fps graph */
	timer.Update();

	float dt = timer.GetDeltaTime();

	fpsGraph.Input(1.0f / dt, timer.GetTime());


	/* User input */
	if (KeyPressed(KEY_SPACE))
	{
		iCurrentGroup = (iCurrentGroup + 1) % (int)GROUPS_COUNT;
	}
	if (KeyPressed(KEY_ENTER))
	{
		eDisplayMode = (eDisplayMode + 1) % E_DISPLAY_MODES;
	}
	if (KeyPressing(KEY_LEFT))
	{
		if ((fShadowExtent -= dt * 300.0f) < fMinShadowExtent)
			fShadowExtent = fMinShadowExtent;
	}
	if (KeyPressing(KEY_RIGHT))
	{
		if ((fShadowExtent += dt * 300.0f) > fMaxShadowExtent)
			fShadowExtent = fMaxShadowExtent;
	}
	if (KeyPressed(KEY_8))
	{
		bUseDoubleStencil = !bUseDoubleStencil;
	}
	if (KeyPressed(KEY_9))
	{
		bLightMove = !bLightMove;
	}
	if (KeyPressed(KEY_0))
	{
		bWireframe = !bWireframe;
	}
	if (KeyPressing(KEY_UP))
	{
		if ((fDistance -= dt * 150.0f) < fMinDistance)
			fDistance = fMinDistance;
	}
	if (KeyPressing(KEY_DOWN))
	{
		if ((fDistance += dt * 150.0f) > fMaxDistance)
			fDistance = fMaxDistance;
	}
	if (ScrollUp())
	{
		if ((fDistance /= 1.05f) < fMinDistance)
			fDistance = fMinDistance;
	}
	if (ScrollDown())
	{
		if ((fDistance *= 1.05f) > fMaxDistance)
			fDistance = fMaxDistance;
	}
	if (KeyPressed(KEY_R))
	{
		bFPSMode = !bFPSMode;
	}

	if (bFPSMode)
	{
		fpsCamera.Update(this, dt);
	}
	else
	{

		if (KeyPressing(KEY_Q))
		{
			light[iCurLight].fLightHeight += 50.0f * dt;
		}
		if (KeyPressing(KEY_E))
		{
			light[iCurLight].fLightHeight -= 50.0f * dt;
		}
		if (KeyPressing(KEY_W))
		{
			light[iCurLight].fLightRadius += dt;
		}
		if (KeyPressing(KEY_S))
		{
			light[iCurLight].fLightRadius -= dt;
		}
	}
	if (KeyPressed(KEY_1))
	{
		if (iNumLights > 1)
			iNumLights--;
	}
	if (KeyPressed(KEY_2))
	{
		if (iNumLights < MAX_LIGHTS)
			iNumLights++;
	}

	
	// Update light positions
	if (bLightMove)
	{
		for (unsigned int i = 0; i < MAX_LIGHTS; i++)
		{
			light[i].fLightAngle += dt;

			light[i].UpdatePos(fMinDistance);
		}
	}

	/* Update camera state */
	y += 0.002f * (GetPointer()->GetMotionX() + GetPointer()->GetInertiaX());
	x -= 0.002f * (GetPointer()->GetMotionY() + GetPointer()->GetInertiaY());
	if (GetPointer()->TimeSinceLastInput() > 10.0f && bLightMove)
		t += dt;

	yRot = y + 20.0f * t;
	xRot = x + 20.0f * sin(t);

	/* Rendering */
	// Clear color and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);

	UpdateModelView();


	glEnable(GL_CULL_FACE);

	// Draw scene (including shadow if enabled)
	RenderScene();

	glDisable(GL_CULL_FACE);

	// Render Light
	for (unsigned int i = 0; i < iNumLights; i++)
	{
		DrawLightMarker(light[i].fPos);
	}

	//glDisable(GL_DEPTH_TEST);
	glClear(GL_DEPTH_BUFFER_BIT);

	DrawCoordinateFrame();

	if (iShowInfo)
	{
		fpsGraph.Draw();

		/* Draw font */
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
		glEnable(GL_TEXTURE_2D);

		glUseProgram(uiProgram[E_LOOKUP]);

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
			ttFont.SDL_GL_RenderText(color, &position, "%.1f",  1.0f / dt);
		}
		else if (iShowInfo == 2)
		{
			ttFont.SDL_GL_RenderText(color, &position, "FPS=%.1f",  1.0f / dt);
			position.y -= position.h * 1.2f;

			ttFont.SDL_GL_RenderText(color, &position, "[LEFT,RIGHT] Extent=%.1f", fShadowExtent);
			position.y -= position.h * 1.2f;

			char text[200];
			CurrentGroupDesc(text);
			ttFont.SDL_GL_RenderText(color, &position, text);
			position.y -= position.h * 1.2f;			

			ttFont.SDL_GL_RenderText(color, &position, "[ENTER] Render Mode=%s",
				DisplayMoreStr[eDisplayMode]);
			position.y -= position.h * 1.2f;

			ttFont.SDL_GL_RenderText(color, &position, "[8] Double Stencil=%d", bUseDoubleStencil ? 1 : 0);
			position.y -= position.h * 1.2f;
			
			ttFont.SDL_GL_RenderText(color, &position, "[0] Wireframe=%d", bWireframe ? 1 : 0);
			position.y -= position.h * 1.2f;
		}

		TTFont::glDisable2D();
		glDisable(GL_BLEND);
	}
	iFrameCounter++;
	
	return true;
}

void Shadows::PreRenderGeometry(GLuint program)
{
	/* Draw mesh */
	glUseProgram(program);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, uiBGTexture);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glCullFace(GL_BACK);
}

void Shadows::PostRenderGeometry(GLuint program)
{
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void Shadows::RenderGeometry(GLuint program)
{
	PreRenderGeometry(program);

	if (program == uiProgram[E_DIFFUSE])
	{
		glUniform1i(GetUniLoc(program, "NumLights"), iNumLights);
		glUniform4fv(GetUniLoc(program, "Color"), 1, GrayColor);
	}
	PrintOpenGLError();

	pRoomMesh->GetVBO()->Render(GL_TRIANGLES);

	if (program == uiProgram[E_DIFFUSE])
	{
		glUniform4fv(GetUniLoc(program, "Color"), 1, WhiteColor);
	}
	RenderCurrentGroup(false);

	PostRenderGeometry(program);
}


void Shadows::RenderCurrentGroup(bool shadowVolume)
{
	vector<int>::iterator iter;
	// Pointer to member function cannot be used here since the return types
	// are different
	if (shadowVolume)
	{
		for (iter = aaiMeshGroup[iCurrentGroup].begin();
			 iter != aaiMeshGroup[iCurrentGroup].end(); iter++)
		{
			pSVMesh[*iter]->GetShadowVolumeVBO()->Render(GLRenderMode());
		}
	}
	else
	{
		for (iter = aaiMeshGroup[iCurrentGroup].begin();
			 iter != aaiMeshGroup[iCurrentGroup].end(); iter++)
		{
			pSVMesh[*iter]->GetVBO()->Render(GLRenderMode());
		}
	}
}

void Shadows::CurrentGroupDesc(char *str)
{
	unsigned int VBOSize = 0;
	unsigned int shadowVBOSize = 0;
	char local[100];
	strcpy(local, "[SPACE] ( ");
	vector<int>::iterator iter;
	for (iter = aaiMeshGroup[iCurrentGroup].begin();
		 iter != aaiMeshGroup[iCurrentGroup].end(); iter++)
	{
		strcat(local, meshes[*iter]);
		strcat(local, " ");
		VBOSize += pSVMesh[*iter]->GetVBO()->GetCount();
		shadowVBOSize += pSVMesh[*iter]->GetShadowVolumeVBO()->GetCount();
	}
	strcat(local, ")");
	sprintf(str, "%s = (%d, %d)", local, VBOSize, shadowVBOSize);
}

void Shadows::ShowShadowVolume(unsigned int lightIndex)
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glUseProgram(uiProgram[E_SHADOW_VOLUME]);

	glEnable(GL_BLEND);

	GLuint program = uiProgram[E_SHADOW_VOLUME];
	glUniform1i(GetUniLoc(program, "LightIndex"), lightIndex);
	glUniform1f(GetUniLoc(program, "ShadowExtent"), fShadowExtent);
	glUniform1i(GetUniLoc(program, "InfiniteShadowVolume"), false);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	RenderCurrentGroup(true);

	glDisable(GL_BLEND);

	glDisableClientState(GL_NORMAL_ARRAY);	
	glDisableClientState(GL_VERTEX_ARRAY);
}

void Shadows::RenderShadowVolumes(unsigned int lightIndex)
{
	// store current OpenGL state
	//glPushAttrib(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | 
	//             GL_POLYGON_BIT | GL_STENCIL_BUFFER_BIT);

	GLuint program = uiProgram[E_SHADOW_VOLUME];
	glUseProgram(program);

	glUniform1i(GetUniLoc(program, "LightIndex"), lightIndex);
	glUniform1f(GetUniLoc(program, "ShadowExtent"), fShadowExtent);
	glUniform1i(GetUniLoc(program, "InfiniteShadowVolume"), false);

	glClear(GL_STENCIL_BUFFER_BIT);

	if (eDisplayMode == E_SHADOW_VOLUMES)
		glEnable(GL_BLEND);
	else
		glColorMask(0, 0, 0, 0); // do not write to the color buffer

	glDepthMask(0); // do not write to the depth (Z) buffer
	glEnable(GL_STENCIL_TEST); // enable stencil testing

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	// set the reference stencil value to 0
	glStencilFunc(GL_ALWAYS, 0, ~0);

	if (bUseDoubleStencil)
	{
		glDisable(GL_CULL_FACE); // cull faces (back or front)

		glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
		glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);

		// Render group of meshes
		RenderCurrentGroup(true);
	}
	else
	{
		glEnable(GL_CULL_FACE); // cull faces (back or front)

		// Pass 1 for Z-fail
		// increment the stencil value on Z fail
		glStencilOp(GL_KEEP, GL_INCR, GL_KEEP);
		// draw only the back faces of the shadow volume
		glCullFace(GL_FRONT);

		// Render group of meshes
		RenderCurrentGroup(true);

		//glStencilFunc(GL_ALWAYS, 0, ~0);
		// Pass 2 for Z-fail
		// decrement the stencil value on Z fail
		glStencilOp(GL_KEEP, GL_DECR, GL_KEEP);
		// draw only the front faces of the shadow volume
		glCullFace(GL_BACK);

		// Render group of meshes
		RenderCurrentGroup(true);
	}



	glDisableClientState(GL_NORMAL_ARRAY);	
	glDisableClientState(GL_VERTEX_ARRAY);

	if (eDisplayMode == E_SHADOW_VOLUMES)
		glDisable(GL_BLEND);
	else
		glColorMask(1, 1, 1, 1); // do not write to the color buffer
	glDepthMask(1); // do not write to the depth (Z) buffer

	// restore OpenGL state
	//glPopAttrib();
}

void Shadows::RenderPlane(GLuint program, const float *color)
{
	float vertexAttrib[] = {
		-1.0f, -1.0f,
		-1.0f, 1.0f,
		1.0f, -1.0f,
		1.0f, 1.0f,
	};

	glEnableClientState(GL_VERTEX_ARRAY);

	glVertexPointer(2, GL_FLOAT, 0, vertexAttrib);

	glUniform4fv(GetUniLoc(program, "Color"), 1, color);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glDisableClientState(GL_VERTEX_ARRAY);

}

void Shadows::RenderScene()
{
	unsigned int i;

	glDepthFunc(GL_LESS);

	switch (eDisplayMode)
	{
	default:
	//case E_NO_SHADOWS:
	//	RenderGeometry(uiProgram[E_DIFFUSE]);
	//	break;
	//case E_SHADOW_VOLUMES:
	//	RenderGeometry(uiProgram[E_DIFFUSE]);
	//	for (i = 0; i < iNumLights; i++)
	//	{
	//		glEnable(GL_LIGHT0 + i);
	//		glLightfv(GL_LIGHT0 + i, GL_POSITION, light[i].fPos);
	//	}
	//	ShowShadowVolume();
	//	break;
	case E_SHADOW_VOLUMES:
	case E_SHADOWS:
		for (i = 0; i < iNumLights; i++)
		{
			glEnable(GL_LIGHT0 + i);
			glLightfv(GL_LIGHT0 + i, GL_POSITION, light[i].fPos);
		}
		/* First, render all geometry with color and depth write enabled */
		RenderGeometry(uiProgram[E_DIFFUSE]);


		/* Then, render the shadow volumes with color and depth write 
		   disabled. Use the Z-fail approach */
		for (i = 0; i < iNumLights; i++)
		{
			RenderShadowVolumes(i);

			/* Finally, render again all the geometry setting a uniform
			   transparency factor affecting only non-zero stencil pixels
			   (areas in shadow) */
			glDepthFunc(GL_EQUAL);

			// Enable Stencil Test
			glEnable(GL_STENCIL_TEST);

			// update the color only where the stencil value is 0
			glStencilFunc(GL_NOTEQUAL, 0, ~0);
			
			glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

			glEnable(GL_BLEND);

			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			//RenderGeometry(uiProgram[E_DIFFUSE]);
			RenderGeometry(uiProgram[E_SHADOW]);

			glDisable(GL_STENCIL_TEST);
			glDisable(GL_BLEND);
			glDepthFunc(GL_LESS);
		}
		break;

	/* 
	  E_STENCIL is a debug mode used to render visually the values in the
	  stencil buffer.
	  Start with same approach as normal stencil rendering, then render 
	  multiple passes corresponding to different values in the stencil buffer.
	*/
	case E_STENCIL:
		for (i = 0; i < iNumLights; i++)
		{
			glEnable(GL_LIGHT0 + i);
			glLightfv(GL_LIGHT0 + i, GL_POSITION, light[i].fPos);
		}

		RenderGeometry(uiProgram[E_DIFFUSE]);

		for (i = 0; i < iNumLights; i++)
		{
			RenderShadowVolumes(i);

			// re-draw the model with the light enabled only where
			// it has previously been drawn
			glDisable(GL_DEPTH_TEST);

			// update the color only where the stencil value is 0
			glEnable(GL_STENCIL_TEST);

			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);		
			glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

			glEnable(GL_BLEND);

			glDisable(GL_CULL_FACE);
			glUseProgram(uiProgram[E_PLANE]);
			for (unsigned int j = 1; j < 5; j++)
			{
				glStencilFunc(GL_EQUAL, j, ~0);

				RenderPlane(uiProgram[E_PLANE], Colors + (j-1) * 4);
			}
			glStencilFunc(GL_GEQUAL, 5, ~0);
			RenderPlane(uiProgram[E_PLANE], Colors + 16 * 4);

			glDisable(GL_STENCIL_TEST);
			glEnable(GL_CULL_FACE);
			glDisable(GL_BLEND);
			glEnable(GL_DEPTH_TEST);
		}
		break;
	}
}


void Shadows::DrawLightMarker(float *lightPos)
{
	float white[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glUseProgram(uiProgram[E_UNIFORM]);
	glUniform4fv(GetUniLoc(uiProgram[E_UNIFORM], "Color"), 1, white);

	glPushMatrix();
	glTranslatef(lightPos[0], lightPos[1], lightPos[2]);
	glScalef(10.0f, 10.0f, 10.0f);
	glEnableClientState(GL_VERTEX_ARRAY);
	pTetraVBO->Render(GL_TRIANGLES);
	glDisableClientState(GL_VERTEX_ARRAY);
	glPopMatrix();
}

void Shadows::DrawCoordinateFrame()
{
	glUseProgram(uiProgram[E_COLOR_OFFSET]);
	float offset[] = { -0.9f, -0.9f };
	glUniform2fv(GetUniLoc(uiProgram[E_COLOR_OFFSET], "Offset"), 1, offset);

	glLoadIdentity();

	glTranslatef(0.0f, 0.0f, -fDefDistance);
	glRotatef(xRot, 1.0f, 0.0f, 0.0f);
	glRotatef(yRot, 0.0f, 1.0f, 0.0f);

	CoordinateFrame::Instance().Render();
}


bool Shadows::ReleaseGL()
{
	CoordinateFrame::Instance().Unload();

	for (unsigned int i = 0; i < MESH_NUM - 1; i++)
	{
		delete pSVMesh[i];
	}
	delete pTetraVBO;
	delete pRoomMesh;

	return GLResourceManager::Instance().Release();
}

bool Shadows::ReleaseApp()
{
	return true;
}

Shell *NewDemo()
{
	return new Shadows();
}

