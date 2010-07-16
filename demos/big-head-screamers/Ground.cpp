/*****************************************************************************
 * Filename			Ground.cpp
 * 
 * License			LGPL
 *
 * Author			Andrea Bizzotto (bizz84@gmail.com)
 *
 * Platform			LinuxX11 / OpenGL
 * 
 * Description		Ground drawing functionality via infinite plane
 *
 *****************************************************************************/

#include "BigHeadScreamers.h"
#include "Ground.h"
#include "Misc.h"

static const char *shaders[] = {
	"data/shaders/Infinite.vert", "data/shaders/Infinite.frag",
};



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

void Ground::Render(const Matrix4 &invProjView, const Vector3 &eyePos,
					const float zfar)
{
	// Only a "fake" position is passed to the InfinitePlane() function
	// The actual xz position is simulated in the vertex shader by
	// adding the xz offset to the texture coordinates
	Vector3 fakePos = Vector3(0.0f, eyePos[1], 0.0f);

	const float plane[] = { 0.0f, 1.0f, 0.0f, 0.0f };
	uiInfPlaneVertices = InfinitePlane(vGround, plane, invProjView,
		fakePos, zfar);

	glEnableClientState(GL_VERTEX_ARRAY);

	GLuint shader = uiProgram[E_INFINITE];

	glUseProgram(shader);

	float texoffset[] = { eyePos[0], eyePos[2] };
	glUniform1f(GetUniLoc(shader, "ZFar"), zfar);
	glUniform1f(GetUniLoc(shader, "TexRepeat"), zfar / 100.0f);
	glUniform2fv(GetUniLoc(shader, "PosOffset"), 1, texoffset);

	glVertexPointer(3, GL_FLOAT, sizeof(Vector3), &vGround);
	glDrawArrays(GL_TRIANGLE_FAN, 0, uiInfPlaneVertices);

	glDisableClientState(GL_VERTEX_ARRAY);	
}
