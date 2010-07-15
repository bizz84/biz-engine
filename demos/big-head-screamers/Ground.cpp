/*****************************************************************************
 * Filename			BigHeadScreamers.cpp
 * 
 * License			LGPL
 *
 * Author			Andrea Bizzotto (bizz84@gmail.com)
 *
 * Platform			LinuxX11 / OpenGL
 * 
 * Description		Lighting & BigHeadScreamers demo using stencil buffers
 *
 *****************************************************************************/

#include "BigHeadScreamers.h"
#include "Ground.h"
#include "Vector.h"
#include "Matrix.h"
#include "Misc.h"

// TODO: remove
#include "UnitTest.h"


static const char *shaders[] = {
	"data/shaders/Lookup.vert", "data/shaders/Lookup.frag",
	"data/shaders/Identity.vert", "data/shaders/Identity.frag",
	"data/shaders/Infinite.vert", "data/shaders/Infinite.frag",
};

#define DRAW_PLANE4(X, T, MODE) \
{\
	float temp[] = {\
		X[0][0], X[0][1], X[0][2], X[0][3], T, T, \
		X[1][0], X[1][1], X[1][2], X[1][3], 0.0f, T,\
		X[2][0], X[2][1], X[2][2], X[2][3], 0.0f, 0.0f,\
		X[3][0], X[3][1], X[3][2], X[3][3], T, 0.0f\
	};\
	glVertexPointer(4, GL_FLOAT, sizeof(float) * 6, temp);\
	glTexCoordPointer(2, GL_FLOAT, sizeof(float) * 6, temp + 4);\
	glDrawArrays(MODE, 0, 4);\
}

#define DRAW_INFINITE_PLANE(X, T, N, MODE) \
{\
	float temp[] = {\
		X[0][0], X[0][1], X[0][2], T, T, \
		X[1][0], X[1][1], X[1][2], 0.0f, T,\
		X[2][0], X[2][1], X[2][2], 0.0f, 0.0f,\
		X[3][0], X[3][1], X[3][2], T, 0.0f\
	};\
	glVertexPointer(3, GL_FLOAT, sizeof(float) * 5, temp);\
	glTexCoordPointer(2, GL_FLOAT, sizeof(float) * 5, temp + 3);\
	glDrawArrays(MODE, 0, N);\
}

Ground::Ground()
	: uiInfPlaneVertices(0)
{

}

bool Ground::Init()
{
	GLResourceManager &loader = GLResourceManager::Instance();
	for (unsigned int i = 0; i < NUM_SHADERS; i++)
	{
		if (!loader.LoadShaderFromFile(shaders[i*2+0], shaders[i*2+1],
			uiProgram[i]))
			return false;
	}
	return true;
}

void Ground::Render(const Matrix4 &invProjView, const Vector3 &eyePos, const float zfar)
{


	//Matrix4 ii = invProjView * projView;
	//Matrix4 ii = invProjView * Matrix4(proj) * Matrix4(mview);
	//invProjView[1][2] = -12.3;
	//invProjView[1][3] = 12.2;
	/*invProjView[2][2] = -34;
	invProjView[2][3] = 33;*/

	Vector3 fakePos = Vector3(0.0f, eyePos[1], 0.0f);

	const float plane[] = { 0.0f, 1.0f, 0.0f, 0.0f };
	uiInfPlaneVertices = InfinitePlane(vGround, plane, invProjView,
		fakePos, zfar);

	glEnableClientState(GL_VERTEX_ARRAY);	
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glUseProgram(uiProgram[E_INFINITE]);

	float texoffset[] = { eyePos[0], eyePos[2] };
	glUniform1f(GetUniLoc(uiProgram[E_INFINITE], "ZFar"), zfar);
	glUniform1f(GetUniLoc(uiProgram[E_INFINITE], "TexRepeat"), zfar / 100.0f);
	glUniform2fv(GetUniLoc(uiProgram[E_INFINITE], "PosOffset"), 1, texoffset);

	float t = 100.0f;
	DRAW_INFINITE_PLANE(vGround, t, uiInfPlaneVertices, GL_TRIANGLE_FAN)

	/*glUseProgram(uiProgram[E_LOOKUP]);
	DRAW_PLANE4(v, t, GL_TRIANGLE_FAN)*/

	/*glUseProgram(uiProgram[E_IDENTITY]);
	float color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glUniform4fv(GetUniLoc(uiProgram[E_IDENTITY], "Color"), 1, color);

	//DRAW_PLANE4(u, t, GL_LINE_LOOP)
	// X has zeroed z component so it's unaffected by depth test
	DRAW_PLANE4(x, t, GL_LINE_LOOP)

	glDisableClientState(GL_VERTEX_ARRAY);	
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);*/
}

void Ground::Projection(Vector4 &u, const float *proj, const float *mview, const Vector4 &v)
{
	u = DebugMatrix4::GLMult(mview, v);
	u = DebugMatrix4::GLMult(proj, u);
}

void Ground::ZeroZ(Vector4 &w, const Vector4 &u)
{
	w = Vector4(u[0], u[1], 0.0f, u[3]);
}

