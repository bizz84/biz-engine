/*****************************************************************************
 * Filename			Timer.cpp
 * 
 * License			LGPL
 *
 * Author			Andrea Bizzotto (bizz84@gmail.com)
 *
 * Platform			LinuxX11 / OpenGL
 * 
 * Description		High resolution timer
 *
 *****************************************************************************/

#include "SkyBox.h"
#include "GLResourceManager.h"
#include "Misc.h"

static const float u = 1.0f;
const float SkyBox::VertexAttrib[3 * 8] = {
	-u, -u, -u, 
	-u, -u,  u, 
	 u, -u,  u,
	 u, -u, -u,
	-u,  u, -u, 
	-u,  u,  u, 
	 u,  u,  u,
	 u,  u, -u
};

const int SkyBox::ElementAttrib[24] = {
	0, 1, 2, 3,
	4, 5, 6, 7,
	0, 1, 5, 4,
	3, 2, 6, 7,
	1, 2, 6, 5,
	0, 3, 7, 4
};

static const char VertexShader[] =
	"varying vec3 TexCoord;" \
	"void main()" \
	"{"\
		"/*gl_TexCoord[0].xyz = gl_Vertex.xyz;*/"\
		"TexCoord = gl_Vertex.xyz;"\
		"gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;"\
	"}";

static const char FragmentShader[] = 
	"uniform samplerCube sTexture;"\
	"varying vec3 TexCoord;"\
	"void main()"\
	"{"\
		"gl_FragColor = textureCube(sTexture, TexCoord);"\
	"}";


SkyBox::~SkyBox()
{
	delete vboCube;
}

SkyBox::SkyBox()
	: init(false),
	vboCube(NULL)
{

}

SkyBox &SkyBox::Instance()
{
	static SkyBox skybox;
	return skybox;
}



bool SkyBox::Init(const char *textures[], const float scale/* = 1.0f*/)
{
	if (init)
		return true;

	fScale = scale;

	GLResourceManager &loader = GLResourceManager::Instance();

	if (!loader.LoadShaderFromMemory(VertexShader, FragmentShader, uiProgram))
		return false;

	//if (!loader.LoadShaderFromFile("data/shaders/Skybox.vert",
	//	"data/shaders/Skybox.frag", uiProgram))
	//	return false;

	glUniform1i(GetUniLoc(uiProgram, "sTexture"), 0);

    SDL_Surface *surface;
    GLenum texture_format;
	GLint  nOfColors;

	glGenTextures(1, &uiSkybox);
	glBindTexture(GL_TEXTURE_CUBE_MAP, uiSkybox);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	for (unsigned int i = 0; i < 6; i++)
	{
		if (!LoadImage(textures[i], surface, texture_format, nOfColors))
			return false;

		glTexImage2D
		(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, 
			0, 
			nOfColors, 
			surface->w,
			surface->h, 
			0, 
			texture_format, 
			GL_UNSIGNED_BYTE, 
			static_cast<const GLubyte *>(surface->pixels)
		);
	}
	//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_GENERATE_MIPMAP, GL_TRUE);

	vboCube = new IndexedVBO((void *)VertexAttrib, sizeof(float) * 3, 8, (void *)ElementAttrib, 24);

	return (init = true);
}

void SkyBox::Render(const float alpha, const float beta)
{
	glDepthMask(0);
	glUseProgram(uiProgram);
	glBindTexture(GL_TEXTURE_CUBE_MAP, uiSkybox);

	glEnableClientState(GL_VERTEX_ARRAY);

	glDisable(GL_CULL_FACE);
	glPushMatrix();
	glLoadIdentity();
	// first rotate beta angle to avoid spin
	glRotatef(beta, 1.0, 0.0, 0.0);
	// then rotate alpha angle to orient
	glRotatef(alpha, 0.0, 1.0, 0.0);

	glScalef(fScale, fScale, fScale);

	vboCube->Render(GL_QUADS);

	//vbo->Render(GL_TRIANGLES);

	glDisableClientState(GL_VERTEX_ARRAY);

	glPopMatrix();

	glDepthMask(1);

}
