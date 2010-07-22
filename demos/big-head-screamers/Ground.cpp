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

static const char *Shaders[] = {
	"data/shaders/Infinite.vert", "data/shaders/Infinite.frag",
};



Ground::Ground(SDLShell *shell)
	: pShell(shell), uiInfPlaneVertices(0)
{

}

bool Ground::Init()
{
	if (!LoadShaders(Shaders, NUM_PROGRAMS))
		return false;
	// set samplers
	GLuint program = Program(P_INFINITE);
	glUseProgram(program);
	glUniform1i(GetUniLoc(program, "sTexture"), 0);
	glUniform1i(GetUniLoc(program, "sTexture2"), 1);
	
	return true;
}

void Ground::Input(const Matrix4 &invProjView,
		const Vector3 &eyePos, const float zfar)
{
	// Only a "fake" position is passed to the InfinitePlane() function
	// The actual xz position is simulated in the vertex shader by
	// adding the xz offset to the texture coordinates
	Vector3 fakePos = Vector3(0.0f, eyePos[1], 0.0f);

	const float plane[] = { 0.0f, 1.0f, 0.0f, 0.0f };
	uiInfPlaneVertices = InfinitePlane(vGround, plane, invProjView, fakePos,
		zfar);
}

void Ground::Render(const Vector3 &eyePos, const float zfar) const
{
	// Additional value for mix computation can be passed to the vertex shader
	/*float arg[5];
	for (unsigned int i = 0; i < uiInfPlaneVertices; i++)
	{
		arg[i] = -Vector4(vGround[i][0], vGround[i][1], vGround[i][2], 1.0f).dot(view[2]) / zfar;
	}*/

	glEnableClientState(GL_VERTEX_ARRAY);

	GLuint shader = Program(P_INFINITE);

	glUseProgram(shader);

	float screeninv[] = { 1.0f / pShell->ShellGet(SDLShell::SHELL_WIDTH),
	                      1.0f / pShell->ShellGet(SDLShell::SHELL_HEIGHT) };
	float texoffset[] = { eyePos[0], eyePos[2] };
	glUniform1f(GetUniLoc(shader, "ZFar"), zfar);
	glUniform1f(GetUniLoc(shader, "TexRepeat"), zfar / 100.0f);
	glUniform2fv(GetUniLoc(shader, "PosOffset"), 1, texoffset);
	glUniform2fv(GetUniLoc(shader, "ScreenInv"), 1, screeninv);

	// Pass additional value as attribute
	// FIXME: Find out why this doesn't work
	//glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, 0, arg);

	// Find out why passing w != 0 in the vertex array doesn't work either
	//glVertexPointer(4, GL_FLOAT, sizeof(Vector4), &vAttrib);
	glVertexPointer(3, GL_FLOAT, sizeof(Vector3), &vGround);
	glDrawArrays(GL_TRIANGLE_FAN, 0, uiInfPlaneVertices);

	glDisableClientState(GL_VERTEX_ARRAY);	
}
